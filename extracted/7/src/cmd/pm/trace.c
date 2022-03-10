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
	unsigned	INT	currpc;
	int		narg, cnt, rcnt;
	unsigned INT	frame, argp;
	unsigned INT	prevfr;
	unsigned INT	indx;
	
	frame = regs[R5] & EVEN;
	currpc = regs[R7];
	rcnt = 0;	/* recursion count limit */
	while(1){
		narg = findroutine(currpc, frame); /* fills in symbol */
		if(errflg){
			printf("non-C function: ");
			rcnt++;
		}
		printf("%.8s(", symbol.n_name);
		argp = frame + 4;
		if(--narg >= 0)
			printf("0%o", core[argp>>1]&0xffff);	/* core is shorts */
		/* due to optimizer, narg may be very wrong */
		cnt = 0;
		while(--narg >= 0){
			argp += 2;
			printf(", 0%o", core[argp>>1]&0xffff);
			if(++cnt > MAXARGS)
				break;
		}
		printf(")");
		if(narg > 0)
			printf(" plus %d possible argument%s",
					narg, narg==1?"":"s");
		
		/* print local symbols someday */
		printf("\n");

		if (frame >= (base - 4)	/* frame ptr is second word in mark */
		   || (prevfr = core[frame>>1]) == INITREG)
			break;

		/* obtain return address; move frame ptr to previous frame */

		currpc = core[(frame+2)>>1];

		frame = prevfr;
		if (frame< (0177777 & proctb[pid].plimit)
			|| frame>= (0177777 &proctb[pid].pbase) ) {
			printf("...possible stack corruption\n");
			break;
		}

		if(core[frame>>1] == 0){/* head of Xinu task */
			findsym(proctb[pid].paddr, N_TEXT);
			printf("%.8s(", symbol.n_name);
			narg = proctb[pid].pargs;
			argp = frame + 4;
			if(--narg >= 0)
				printf("0%o", core[argp>>1]&0xffff);
			/* due to optimizer, narg may be very wrong */
			cnt = 0;
			while(--narg >= 0){
				argp += 2;
				printf(", 0%o", core[argp>>1]&0xffff);
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

findroutine(currpc, cframe)
unsigned INT	cframe;
unsigned INT	currpc;
{
	register INT	inst;
	register INT	callinst;
	unsigned	INT	instaddr;
	int	nargs;

	errflg = FALSE;
	

	if(findsym(currpc, N_TEXT) == -1) {
		errflg = TRUE;
		symbol.n_name[0] = '?';
		symbol.n_name[1] = 0;
		symbol.n_value = 0;
	}

	instaddr = 0177777 & core[(cframe+2)>>1];
	inst = 0177777 & text[instaddr >> 1];
	callinst = 0177777 & text[(instaddr-4) >> 1];

	if(callinst == 04737)		/* apparent compiler convention */
		nargs = 1;
	else
		nargs = 0;
	if(inst == 05726)		/* tst (sp)+ */
		return(nargs + 1);
	if(inst == 022626)		/* cmp (sp)+,(sp)+ */
		return(nargs + 2);
	if(inst == 062706)		/* add $n,sp */
		/* convert n bytes to words and return */
		return(nargs +  text[(instaddr+2) >>1] >>1);
	return(nargs);
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
