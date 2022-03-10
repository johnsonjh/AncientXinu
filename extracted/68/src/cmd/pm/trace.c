#include "pm.h"

/*
 * pm - traceback routines
 */


struct nlist	symbol;
INT		lastframe;
INT		lastpc;
INT		cntflg;
INT		cntval;

/* symbol management */
L_INT		localval;

struct reglist reglist [] = {
		"ps", PS, 
		"pc", R7,
		"sp", R6,
		"r5", R5,
		"r4", R4,
		"r3", R3,
		"r2", R2,
		"r1", R1,
		"r0", R0,
};

char		lastc;
extern	struct	pentry	*proctb;

/* general printing routines  */

printtrace(regs, base, pid)
unsigned INT	regs[NREGS];
unsigned INT	base;
unsigned INT	pid;
{
	int		narg, cnt, rcnt;
	unsigned INT	frame, callpc, argp;
	
	frame = regs[R5] & EVEN;
	callpc = regs[R7];
	rcnt = 0;	/* recursion count limit */
	while(1){
		narg = findroutine(frame); /* fills in symbol */
		if(errflg){
			printf("non-C function: ");
			rcnt++;
		}
		printf("%.8s(", symbol.n_name);
		argp = frame + 4;
		if(--narg >= 0)
			printf("0%o", core[argp>>1]&0xff);	/* core is shorts */
		/* due to optimizer, narg may be very wrong */
		cnt = 0;
		while(--narg >= 0){
			argp += 2;
			printf(", 0%o", core[argp>>1]&0xff);
			if(++cnt > MAXARGS)
				break;
		}
		printf(")");
		if(narg > 0)
			printf(" plus %d possible argument%s",
					narg, narg==1?"":"s");
		
		/* print local symbols someday */
		printf("\n");

		if(frame >= base - 6)	/* frame pointer is second word in mark */
			break;

		frame = core[frame>>1];

		if(core[frame>>1] == 0){/* head of Xinu task */
			findsym(proctb[pid].paddr, N_TEXT);
			printf("%.8s(", symbol.n_name);
			narg = proctb[pid].pargs;
			argp = frame + 4;
			if(--narg >= 0)
				printf("0%o", core[argp>>1]&0xff);
			/* due to optimizer, narg may be very wrong */
			cnt = 0;
			while(--narg >= 0){
				argp += 2;
				printf(", 0%o", core[argp>>1]&0xff);
				if(++cnt > MAXARGS)
					break;
			}
			printf(")");
			if(narg > 0)
				printf(" plus %d possible argument%s",
						narg, narg==1?"":"s");
			
			return;
		}
		if(rcnt > RECLIMIT){
			printf("recursion limit of %d reached\n", RECLIMIT);
			return;
		}
	}
}

printregs(regs)
unsigned INT	regs[NREGS];
{
	register struct reglist *p;
	INT			v;

	for(p = reglist; p < &reglist[NREGS]; p++){
		v = regs[p->roffs];
		printf("%.8s\t0%o\t", p->rname, v & 0xffff);
		if(p->roffs != PS)
			valpr((INT)v,(p->roffs == R7?N_TEXT:N_DATA));
		printf("\n");
	}
/*	printpc(regs);*/
}

/*
 * Print a value v symbolically, if it has a reasonable
 * interpretation as name+offset. If not, print nothing.
 */

valpr(v, idsp)
unsigned INT	v;
{
	long	findsym(), d;

	d = findsym(v, idsp);
	if(d >= MAXOFF)
		return;
	printf("%.8s", symbol.n_name);
	if(d)
		printf("+0%o", d&0xffff);
}

/*
 * printpc - print the word pointed to by the pc both
 * symbolically and as an instruction
 */

printpc(regs)
INT	regs[NREGS];
{
	psymoff(regs[R7], ":\t", N_TEXT);
/*	printins(regs[R7], regs);*/
}

/*
 * findroutine - find the name of the routine pointed
 * to by the current frame. Leave its symbol table entry in
 * symbol as a side effect; returns the number of arguments 
 * used in the call, after horrible kludging around to find it.
 *
 * because the optimizer may change the mechanism used to pop
 * the arguments off the stack, the value returned may be
 * in error.
 */

findroutine(cframe)
unsigned INT	cframe;
{
	register INT	narg, inst;
	unsigned INT	callpc, back2;
	boolean		v;

	v=FALSE; 
	errflg = FALSE;
	lastpc=callpc;

	callpc=core[(cframe+2)>>1];
	back2=core[(callpc-2)>>1];

	if((inst = core[(callpc-4)>>1]) == 04737)	/* jsr pc,*$... */
		narg = 1;
	else if((inst&~077)==04700){			/* jsr pc,... */
		narg=0;
		v=(inst!=04767);
	}else if((back2&~077)==04700){
		narg=0;
		v=TRUE;
	}else{
		errflg=NOCFN;
		return(0);
	}

	if(findsym(
		(v ? lastpc : ((inst==04767?callpc:0) + back2) ),
		    N_TEXT) == -1 && !v){
		symbol.n_name[0] = '?';
		symbol.n_name[1] = 0;
		symbol.n_value = 0;
	}

	inst = text[callpc>>1];

	if(inst == 05726)		/* tst (sp)+ */
		return(narg+1);
	if(inst == 022626)		/* cmp (sp)+,(sp)+ */
		return(narg+2);
	if(inst == 062706)		/* add $n,sp */
		return(narg+text[(callpc+2)>>1]);
	return(narg);
}

/*
 * find the closest symbol to val, and return the 
 * difference between val and the symbol found.
 * leave the symbol table entry in 'symbol' as a side effect.
 */

long
findsym(val, type)
INT	val;
INT	type;
{
	long			diff;
	register struct nlist	*sp, *cursym;

	diff = 0377777L;
	cursym = &symbol;
	symbol.n_name[0] = 0;
	symbol.n_name[1] = 0;
	symbol.n_value = 0;

	for(sp = ssymtab; sp < essymtab; sp++){
		switch(type){
		case N_TEXT:
			if((sp->n_type & 07) != type)
				continue;
			break;

		case N_DATA:
			if((sp->n_type & 07) != N_DATA 
			   && (sp->n_type & 07) != N_BSS)
				break;
			continue;

		default:
			fprintf(stderr, "bad type in findsym\n");
		}
		if(sp->n_value <= val)
			cursym = sp;
		else{
			if(cursym->n_name[0])
				diff = val - cursym->n_value;
			break;
		}
	}
	if(cursym->n_name[0])
		symbol = *cursym;
	else
		symbol.n_name[0] = '?';
	return(diff);
}

/*
 * psymoff - basically call findsym and print the result
 */

psymoff(val, str, space)
INT	val;
char	*str;
{
	long findsym(), d;

	d = findsym(val, space);
	if(d > MAXOFF)
		printf("0%o", val&0xffff);
	else{
		printf("%.8s", symbol.n_name);
		if(d)
			printf("+0%o", d&0xffff);
	}
	printf(str);
}
