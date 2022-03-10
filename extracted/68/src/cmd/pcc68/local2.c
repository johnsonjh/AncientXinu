static char *sccsid ="%W% (Berkeley) %G%";
# include "mfile2"
# include "ctype.h"
# ifdef FORT
int ftlab1, ftlab2;
# endif
/* a lot of the machine dependent parts of the second pass */
extern int fltfun;
# define BITMASK(n) ((1L<<n)-1)

where(c)
{
	fprintf( stderr, "%s, line %d: ", filename, lineno );
}

lineid( l, fn ) char *fn; 
{
	/* identify line l and file fn */
#ifdef VAX
	printf( "#	line %d, file %s\n", l, fn );
#else
	printf( "|	line %d, file %s\n", l, fn );
#endif
}

#ifndef VAX
int	usedregs;		/* Flag word for registers used in current routine */
int maxtoff;

cntbits(i)
  register int i;
{	register int j,ans;

	for (ans=0, j=0; i!=0 && j<16; j++) { if (i&1) ans++; i >>= 1 ; }
	return(ans);
}
#endif

eobl2()
{
	OFFSZ spoff;	/* offset from stack pointer */
#ifndef FORT
	extern int ftlab1, ftlab2;
#endif

	spoff = maxoff;
#ifdef VAX
	if( spoff >= AUTOINIT ) spoff -= AUTOINIT;
#endif
	spoff /= SZCHAR;
#ifdef VAX
	SETOFF(spoff,4);
#else
	SETOFF(spoff,2);
#endif
#ifdef FORT
#ifndef FLEXNAMES
#ifdef VAX
	printf("	.set	.F%d,%ld\n",ftnno,spoff);
#else
	printf("	LF%d=%ld\n",ftnno,spoff);
#endif
#else
#ifdef VAX
	/* Should be L%d ... ftnno, but must change pc/f77 */
	printf("	.set	LF%d,%ld\n",ftnno,spoff);
#else
	printf("	LF%d=%ld\n",ftnno,spoff);
#endif
#endif
#else
#ifdef VAX
	printf( "L%d:\n", ftlab1);
	if( spoff!=0 )
		if( spoff < 64 )
			printf( "	subl2	$%ld,sp\n", spoff);
		else
			printf( "	movab	-%ld(sp),sp\n", spoff);
	printf( "	jbr 	L%d\n", ftlab2);
	maxargs = -1;
#else
	usedregs &= 036374;	/* only save reg used for reg vars */
	spoff += 4*cntbits(usedregs); /* save at base of stack frame */
	printf( "LE%d:\n",ftnno);
	if (spoff > 32767){
		printf("	movl	#-LF%d,d%d\n", ftnno, fltfun?2:1);
		printf("	moveml	a6@(0,d%d:l),#LS%d\n", fltfun?2:1, ftnno);
	}
	else
		printf( "	moveml	a6@(-LF%d),#LS%d\n", ftnno, ftnno );
	printf( "	unlk	a6\n");
	printf( "	rts\n" );
	printf( "   LF%d = %ld\n", ftnno, spoff );
	if (spoff > 32767)
		printf( "	LS%d = 0x%x\n", ftnno, usedregs|(fltfun?0x8:0x0) );
	else
		printf( "	LS%d = 0x%x\n", ftnno, usedregs );
	maxtoff = 0;
	if( fltused ) {
		fltused = 0;
		printf( "	.globl	fltused\n" );
		}
	printf( "L%d:\n", ftlab1);
	printf( "	bra 	L%d\n", ftlab2);
#endif
#endif
}

struct hoptab { int opmask; char * opstring; } ioptab[] = {

#ifdef VAX
	ASG PLUS, "add",
	ASG MINUS, "sub",
	ASG MUL, "mul",
	ASG DIV, "div",
	ASG OR, "bis",
	ASG ER,	"xor",
	ASG AND, "bic",
	PLUS,	"add",
	MINUS,	"sub",
	MUL,	"mul",
	DIV,	"div",
	OR,	"bis",
	ER,	"xor",
	AND,	"bic",
#else
	ASG PLUS, "add",
	ASG MINUS, "sub",
	ASG OR,	"or",
	ASG AND, "and",
	ASG ER,	"eor",
	ASG MUL, "mul",
	ASG DIV, "div",
	ASG MOD, "div",
	ASG LS,	"sl",
	ASG RS,	"sr",
#endif
	-1, ""    };

hopcode( f, o )
{
	/* output the appropriate string from the above table */

	register struct hoptab *q;

	for( q = ioptab;  q->opmask>=0; ++q ){
		if( q->opmask == o ){
			printf( "%s", q->opstring );
#ifndef VAX
			if( f == 'F') printf("f");
#else
/* tbl
			if( f == 'F' ) printf( "e" );
			else if( f == 'D' ) printf( "d" );
   tbl */
/* tbl */
			switch( f ) {
				case 'L':
				case 'W':
				case 'B':
				case 'D':
				case 'F':
					printf("%c", tolower(f));
					break;

				}
/* tbl */
#endif
			return;
			}
		}
	cerror( "no hoptab for %s", opst[o] );
}

char *
rnames[] = {  /* keyed to register number tokens */

#ifdef VAX
	"r0", "r1",
	"r2", "r3", "r4", "r5",
	"r6", "r7", "r8", "r9", "r10", "r11",
	"ap", "fp", "sp", "pc",
#else
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp"
#endif
	};

int rstatus[] = {
#ifdef VAX
	SAREG|STAREG, SAREG|STAREG,
	SAREG|STAREG, SAREG|STAREG, SAREG|STAREG, SAREG|STAREG,
	SAREG, SAREG, SAREG, SAREG, SAREG, SAREG,
	SAREG, SAREG, SAREG, SAREG,
#else
	SAREG|STAREG,	SAREG|STAREG,
	SAREG|STAREG,	SAREG|STAREG,
	SAREG|STAREG,	SAREG|STAREG,
	SAREG|STAREG,	SAREG|STAREG,

	SBREG|STBREG,	SBREG|STBREG,
	SBREG|STBREG,	SBREG|STBREG,
	SBREG|STBREG,	SBREG|STBREG,
	SBREG,			SBREG,
#endif
	};

tlen(p) NODE *p;
{
	switch(p->in.type) {
		case CHAR:
		case UCHAR:
			return(1);

		case SHORT:
		case USHORT:
			return(2);

		case DOUBLE:
			return(8);

		default:
			return(4);
		}
}

#ifdef VAX

mixtypes(p, q) NODE *p, *q;
{
	register tp, tq;

	tp = p->in.type;
	tq = q->in.type;

	return( (tp==FLOAT || tp==DOUBLE) !=
		(tq==FLOAT || tq==DOUBLE) );
}

prtype(n) NODE *n;
{
	switch (n->in.type)
		{
		case DOUBLE:
			printf("d");
			return;

		case FLOAT:
			printf("f");
			return;

		case LONG:
		case ULONG:
		case INT:
		case UNSIGNED:
			printf("l");
			return;

		case SHORT:
		case USHORT:
			printf("w");
			return;

		case CHAR:
		case UCHAR:
			printf("b");
			return;

		default:
			if ( !ISPTR( n->in.type ) ) cerror("zzzcode- bad type");
			else {
				printf("l");
				return;
				}
		}
}

zzzcode( p, c ) register NODE *p; 
{
	register m;
	CONSZ val;
	switch( c ){

	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		cbgen( 0, m=getlab(), 'I' );
		deflab( p->bn.label );
		printf( "	clrl	%s\n", temp = rnames[getlr( p, '1' )->tn.rval] );
		usedregs |= 1<<temp;
		deflab( m );
		return;

	case 'I':
		cbgen( p->in.op, p->bn.label, c );
		return;

	/* stack management macros */
	case '-':
		printf( "sp@-");
	case 'P':
		toff += 4;
		if (toff > maxtoff) maxtoff = toff;
		return;

	case '0':
		toff = 0; return;
	#endif

	case 'A':
		{
		register NODE *l, *r;

		if (xdebug) eprint(p, 0, &val, &val);
		r = getlr(p, 'R');
		if (optype(p->in.op) == LTYPE || p->in.op == UNARY MUL)
			{
			l = resc;
			l->in.type = (r->in.type==FLOAT || r->in.type==DOUBLE ? DOUBLE : INT);
			}
		else
			l = getlr(p, 'L');
		if (r->in.op == ICON)
			if(r->in.name[0] == '\0')
				{
				if (r->tn.lval == 0)
					{
					printf("clr");
					prtype(l);
					printf("	");
					adrput(l);
					return;
					}
				if (r->tn.lval < 0 && r->tn.lval >= -63)
					{
					printf("mneg");
					prtype(l);
					r->tn.lval = -r->tn.lval;
					goto ops;
					}
				r->in.type = (r->tn.lval < 0 ?
						(r->tn.lval >= -128 ? CHAR
						: (r->tn.lval >= -32768 ? SHORT
						: INT )) : r->in.type);
				r->in.type = (r->tn.lval >= 0 ?
						(r->tn.lval <= 63 ? INT
						: ( r->tn.lval <= 127 ? CHAR
						: (r->tn.lval <= 255 ? UCHAR
						: (r->tn.lval <= 32767 ? SHORT
						: (r->tn.lval <= 65535 ? USHORT
						: INT ))))) : r->in.type );
				}
				else
					{
					printf("moval");
					printf("	");
					acon(r);
					printf(",");
					adrput(l);
					return;
					}

		if (l->in.op == REG && l->in.type != FLOAT && l->in.type != DOUBLE)
			{
			if( tlen(l) < tlen(r) )
				{
				if (!mixtypes(l,r))
					{
					!ISUNSIGNED(l->in.type)?
						printf("cvt"):
						printf("movz");
					prtype(l);
					printf("l");
					goto ops;
					}
				else
					{
					printf("cvt");
					prtype(r);
					prtype(l);
					printf("	");
					adrput(r);
					printf(",");
					adrput(l);
					printf("cvt");
					prtype(l);
					printf("l");
					printf("	");
					adrput(l);
					printf(",");
					adrput(l);
					return;
					}
				}
			else
				{
			l->in.type = INT;
				}
			}
		if (!mixtypes(l,r))
			{
			if (tlen(l) == tlen(r))
				{
				printf("mov");
				prtype(l);
				goto ops;
				}
			else if (tlen(l) > tlen(r) && ISUNSIGNED(r->in.type))
				{
				printf("movz");
				}
			else
				{
				printf("cvt");
				}
			}
		else
			{
			printf("cvt");
			}
		prtype(r);
		prtype(l);
	ops:
		printf("	");
		adrput(r);
		printf(",");
		adrput(l);
		return;
		}

	case 'B':	/* get oreg value in temp register for left shift */
		{
		register NODE *r;
		if (xdebug) eprint(p, 0, &val, &val);
		r = p->in.right;
		if( tlen(r) == sizeof(int) && r->in.type != FLOAT )
			printf("movl");
		else {
			printf("cvt");
			prtype(r);
			printf("l");
			}
		return;
		}

	case 'C':	/* num words pushed on arg stack */
		{
		extern int gc_numbytes;
		extern int xdebug;

		if (xdebug) printf("->%d<-",gc_numbytes);

		printf("$%d", gc_numbytes/(SZLONG/SZCHAR) );
		return;
		}

	case 'D':	/* INCR and DECR */
		zzzcode(p->in.left, 'A');
		printf("\n	");

	case 'E':	/* INCR and DECR, FOREFF */
		if (p->in.right->tn.lval == 1)
			{
			printf("%s", (p->in.op == INCR ? "inc" : "dec") );
			prtype(p->in.left);
			printf("	");
			adrput(p->in.left);
			return;
			}
		printf("%s", (p->in.op == INCR ? "add" : "sub") );
		prtype(p->in.left);
		printf("2	");
		adrput(p->in.right);
		printf(",");
		adrput(p->in.left);
		return;

	case 'F':	/* register type of right operand */
		{
		register NODE *n;
		extern int xdebug;
		register int ty;

		n = getlr( p, 'R' );
		ty = n->in.type;

		if (xdebug) printf("->%d<-", ty);

		if ( ty==DOUBLE) printf("d");
		else if ( ty==FLOAT ) printf("f");
		else printf("l");
		return;
		}

	case 'L':	/* type of left operand */
	case 'R':	/* type of right operand */
		{
		register NODE *n;
		extern int xdebug;

		n = getlr ( p, c);
		if (xdebug) printf("->%d<-", n->in.type);

		prtype(n);
		return;
		}

	case 'Z':	/* complement mask for bit instr */
		printf("$%ld", ~p->in.right->tn.lval);
		return;

	case 'U':	/* 32 - n, for unsigned right shifts */
		printf("$%d", 32 - p->in.right->tn.lval );
		return;

	case 'T':	/* rounded structure length for arguments */
		{
		int size;

		size = p->stn.stsize;
		SETOFF( size, 4);
		printf("$%d", size);
		return;
		}

	case 'S':  /* structure assignment */
		{
			register NODE *l, *r;
			register size;

			if( p->in.op == STASG ){
				l = p->in.left;
				r = p->in.right;

				}
			else if( p->in.op == STARG ){  /* store an arg into a temporary */
				l = getlr( p, '3' );
				r = p->in.left;
				}
			else cerror( "STASG bad" );

			if( r->in.op == ICON ) r->in.op = NAME;
			else if( r->in.op == REG ) r->in.op = OREG;
			else if( r->in.op != OREG ) cerror( "STASG-r" );

			size = p->stn.stsize;

			if( size <= 0 || size > 65535 )
				cerror("structure size <0=0 or >65535");

			switch(size) {
				case 1:
					printf("	movb	");
					break;
				case 2:
					printf("	movw	");
					break;
				case 4:
					printf("	movl	");
					break;
				case 8:
					printf("	movq	");
					break;
				default:
					printf("	movc3	$%d,", size);
					break;
			}
			adrput(r);
			printf(",");
			adrput(l);
			printf("\n");

			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;

			}
		break;

	default:
		cerror( "illegal zzzcode" );
		}
}

#else
	/* zzzcode for the 68000 */

NODE *brnode;
int brcase;

int toff = 0; /* number of stack locations used for args */

zzzcode( p, c ) NODE *p; 
{

	register m,temp;
	switch( c ){

	case 'C':
		switch (p->in.left->in.op) {

#ifdef FORT
	/* before each jsr push the argument pointer AP on the stack
	   and set it to point to that spot on the stack. This makes
	   the AP point to the same spot it does when using the vax
	   calls instruction. after the jump it is necessary to re-
	   store the AP */

		  case ICON:
				printf("\tmovl\tap,sp@-\n");
				printf("\tmovl\tsp,ap\n");
		    	printf("\tjsr\t");
				acon(p->in.left);
				printf("\tmovl\tsp@+,ap\n");
				return;

		  case REG:	
				printf("\tmovl\tap,sp@-\n");
				printf("\tmovl\tsp,ap\n");
		    	printf("\tjsr\t");
				adrput(p->in.left);
				printf("@");
				printf("\tmovl\tsp@+,ap\n");
				return;

		  case NAME:
		  case OREG:	
				printf("\tmovl\tap,sp@-\n");
				printf("\tmovl\tsp,ap\n");
				printf("\tmovl\t");
				adrput(p->in.left);
				printf(",a0\n\tjsr\ta0@");
				printf("\tmovl\tsp@+,ap\n");
				return;
#else
		  case ICON:	printf("\tjsr\t");
				acon(p->in.left);
				return;

		  case REG:	printf("\tjsr\t");
				adrput(p->in.left);
				printf("@");
				return;

		  case NAME:
		  case OREG:	printf("\tmovl\t");
				adrput(p->in.left);
				printf(",a0\n\tjsr\ta0@");
				return;
#endif FORT

		  default:	cerror("bad subroutine name");
		}

	case 'L':
		m = p->in.left->in.type;
		goto suffix;

	case 'R':
		m = p->in.right->in.type;
		goto suffix;

	case 'B':
		m = p->in.type;		/* fall into suffix: */

	suffix:	if( m == CHAR || m == UCHAR ) printf( "b" );
		else if( m == SHORT || m == USHORT ) printf( "w" );
		else printf( "l" );
		return;


	case 'N':  /* logical ops, turned into 0-1 */
		/* use register given by register 1 */
		cbgen( 0, m=getlab(), 'I' );
		deflab( p->bn.label );
		printf( "	clrl	%s\n", rnames[temp = getlr( p, '1' )->tn.rval] );
		usedregs |= 1<<temp;
		deflab( m );
		return;

	case 'I':
		cbgen( p->in.op, p->bn.label, c );
		return;

		/* stack management macros */
	case '-':
		printf( "sp@-" );
	case 'P':
		toff += 4;
		if (toff > maxtoff) maxtoff = toff;
		return;

	case '0':
		toff = 0; return;

	case '~':
		/* complimented CR */
		p->in.right->tn.lval = ~p->in.right->tn.lval;
		conput( getlr( p, 'R' ) );
		p->in.right->tn.lval = ~p->in.right->tn.lval;
		return;

	case 'M':
		/* negated CR */
		p->in.right->tn.lval = -p->in.right->tn.lval;
	case 'O':
		conput( getlr( p, 'R' ) );
		p->in.right->tn.lval = -p->in.right->tn.lval;
		return;

	case 'T':
		/* Truncate longs for type conversions:
		    INT|UNSIGNED -> CHAR|UCHAR|SHORT|USHORT
		   increment offset to second word */

		m = p->in.type;
		p = p->in.left;
		switch( p->in.op ){
		case NAME:
		case OREG:
			if (p->in.type==SHORT || p->in.type==USHORT)
			  p->tn.lval += (m==CHAR || m==UCHAR) ? 1 : 0;
			else p->tn.lval += (m==CHAR || m==UCHAR) ? 3 : 2;
			return;
		case REG:
			return;
		default:
			cerror( "Illegal ZT type conversion" );
			return;

			}

	case 'U':
		cerror( "Illegal ZU" );
		/* NO RETURN */

	case 'W':	/* structure size */
		if( p->in.op == STASG )
			printf( "%d", p->stn.stsize);
		else	cerror( "Not a structure" );
		return;

	case 'S':  /* structure assignment */
		{
			register NODE *l, *r;
			register size, i;

			if( p->in.op == STASG ){
				l = p->in.left;
				r = p->in.right;
				}
			else if( p->in.op == STARG ){  /* store an arg onto the stack */
				r = p->in.left;
				}
			else cerror( "STASG bad" );

			if( r->in.op == ICON ) r->in.op = NAME;
			else if( r->in.op == REG ) r->in.op = OREG;
			else if( r->in.op != OREG ) cerror( "STASG-r" );

			size = p->stn.stsize;

			r->tn.lval += size;
			l->tn.lval += size;

			while( size ){ /* simple load/store loop */
				i = (size > 2) ? 4 : 2;
				r->tn.lval -= i;
				expand( r, FOREFF,(i==2)?"\tmovw\tAR,":"\tmovl\tAR," );
				l->tn.lval -= i;
				expand( l, FOREFF, "AR\n" );
				size -= i;
			}

			if( r->in.op == NAME ) r->in.op = ICON;
			else if( r->in.op == OREG ) r->in.op = REG;

			}
		break;

	default:
		cerror( "illegal zzzcode" );
		}
}

#endif

rmove( rt, rs, t )
{
#ifdef VAX
	printf( "	%s	%s,%s\n",
		(t==FLOAT ? "movf" : (t==DOUBLE ? "movd" : "movl")),
		rnames[rs], rnames[rt] );
#else
	printf( "	%s	%s,%s\n",
		((t==FLOAT||t==DOUBLE)? "movf" : "movl"),
		rnames[rs], rnames[rt] );
	usedregs |= 1<<rs;
	usedregs |= 1<<rt;
#endif
}

struct respref
respref[] = {
#ifdef VAX
	INTAREG|INTBREG,	INTAREG|INTBREG,
	INAREG|INBREG,	INAREG|INBREG|SOREG|STARREG|STARNM|SNAME|SCON,
	INTEMP,	INTEMP,
	FORARG,	FORARG,
	INTEMP,	INTAREG|INAREG|INTBREG|INBREG|SOREG|STARREG|STARNM,
#else
	INTAREG|INTBREG,	INTAREG|INTBREG,
	INAREG|INBREG,	INAREG|INBREG|SOREG|STARREG|STARNM|SNAME|SCON,
	INTEMP,	INTEMP,
	FORARG,	FORARG,
	INTAREG,	SOREG|SNAME,
#endif
	0,	0 };

setregs()
{ /* set up temporary registers */
#ifdef VAX
	fregs = 6;	/* tbl- 6 free regs on VAX (0-5) */
	;
#else
	register i;
	register int naregs = (maxtreg>>8)&0377;

	/* use any unused variable registers as scratch registers */
	maxtreg &= 0377;
	fregs = maxtreg>=MINRVAR ? maxtreg+1 : MINRVAR;
	if( xdebug ){
		/* -x changes number of free regs to 2, -xx to 3, etc. */
		if( (xdebug+1) < fregs ) fregs = xdebug+1;
		}

	for( i=MINRVAR; i<=MAXRVAR; i++ )
		rstatus[i] = i<fregs ? SAREG|STAREG : SAREG;
	for( i=MINRVAR; i<=MAXRVAR; i++ )
		rstatus[i+8] = i<naregs ? SBREG|STBREG : SBREG;
#endif
}

szty(t)
{ /* size, in registers, needed to hold thing of type t */
#ifdef VAX
	return( (t==DOUBLE||t==FLOAT) ? 2 : 1 );
#else 
	return(t==DOUBLE ? 2 : 1);
#endif
}

rewfld( p ) NODE *p; 
{
	return(1);
}

callreg(p) NODE *p; 
{
#ifdef VAX
	return( R0 );
#else
	return(D0);
#endif
}
	/* BEGIN haven't touched yet */

#ifdef VAX
base( p ) register NODE *p; 
{
	register int o = p->in.op;

	if( (o==ICON && p->in.name[0] != '\0')) return( 100 ); /* ie no base reg */
	if( o==REG ) return( p->tn.rval );
    if( (o==PLUS || o==MINUS) && p->in.left->in.op == REG && p->in.right->in.op==ICON)
		return( p->in.left->tn.rval );
    if( o==OREG && !R2TEST(p->tn.rval) && (p->in.type==INT || p->in.type==UNSIGNED || ISPTR(p->in.type)) )
		return( p->tn.rval + 0200*1 );
	if( o==INCR && p->in.left->in.op==REG ) return( p->in.left->tn.rval + 0200*2 );
	if( o==ASG MINUS && p->in.left->in.op==REG) return( p->in.left->tn.rval + 0200*4 );
	if( o==UNARY MUL && p->in.left->in.op==INCR && p->in.left->in.left->in.op==REG
	  && (p->in.type==INT || p->in.type==UNSIGNED || ISPTR(p->in.type)) )
		return( p->in.left->in.left->tn.rval + 0200*(1+2) );
	return( -1 );
}

offset( p, tyl ) register NODE *p; int tyl; 
{

	if( tyl==1 && p->in.op==REG && (p->in.type==INT || p->in.type==UNSIGNED) ) return( p->tn.rval );
	if( (p->in.op==LS && p->in.left->in.op==REG && (p->in.left->in.type==INT || p->in.left->in.type==UNSIGNED) &&
	      (p->in.right->in.op==ICON && p->in.right->in.name[0]=='\0')
	      && (1<<p->in.right->tn.lval)==tyl))
		return( p->in.left->tn.rval );
	return( -1 );
}

makeor2( p, q, b, o) register NODE *p, *q; register int b, o; 
{
	register NODE *t;
	register int i;
	NODE *f;

	p->in.op = OREG;
	f = p->in.left; 	/* have to free this subtree later */

	/* init base */
	switch (q->in.op) {
		case ICON:
		case REG:
		case OREG:
			t = q;
			break;

		case MINUS:
			q->in.right->tn.lval = -q->in.right->tn.lval;
		case PLUS:
			t = q->in.right;
			break;

		case INCR:
		case ASG MINUS:
			t = q->in.left;
			break;

		case UNARY MUL:
			t = q->in.left->in.left;
			break;

		default:
			cerror("illegal makeor2");
	}

	p->tn.lval = t->tn.lval;
#ifndef FLEXNAMES
	for(i=0; i<NCHNAM; ++i)
		p->in.name[i] = t->in.name[i];
#else
	p->in.name = t->in.name;
#endif

	/* init offset */
	p->tn.rval = R2PACK( (b & 0177), o, (b>>7) );

	tfree(f);
	return;
}

canaddr( p ) NODE *p; 
{
	register int o = p->in.op;

	if( o==NAME || o==REG || o==ICON || o==OREG || (o==UNARY MUL && shumul(p->in.left)) ) return(1);
	return(0);
}

#endif
	/* END haven't touched yet */

shltype( o, p ) register NODE *p; 
{
	return( o== REG || o == NAME || o == ICON || o == OREG || ( o==UNARY MUL && shumul(p->in.left)) );
}

flshape( p ) register NODE *p; 
{
#ifdef VAX
	return( p->in.op == REG || p->in.op == NAME || p->in.op == ICON ||
		(p->in.op == OREG && (!R2TEST(p->tn.rval) || tlen(p) == 1)) );
#else
	register o = p->in.op;
	if( o==NAME || o==REG || o==ICON || o==OREG ) return( 1 );
	return( o==UNARY MUL && shumul(p->in.left)==STARNM );
#endif
}

shtemp( p ) register NODE *p; 
{
#ifdef VAX
	if( p->in.op == STARG ) p = p->in.left;
	return( p->in.op==NAME || p->in.op ==ICON || p->in.op == OREG || (p->in.op==UNARY MUL && shumul(p->in.left)) );
#else
	if( p->in.op == UNARY MUL ) p = p->in.left;
	if( p->in.op == REG || p->in.op == OREG ) return( !istreg( p->tn.rval ) );
	return( p->in.op == NAME || p->in.op == ICON );
#endif
}

#ifndef VAX
spsz( t, v ) TWORD t; CONSZ v; 
{

	/* is v the size to increment something of type t */

	if( !ISPTR(t) ) return( 0 );
	t = DECREF(t);

	if( ISPTR(t) ) return( v == 4 );

	switch( t ){

	case UCHAR:
	case CHAR:
		return( v == 1 );

	case SHORT:
	case USHORT:
		return( v == 2 );

	case INT:
	case UNSIGNED:
	case FLOAT:
		return( v == 4 );

	case DOUBLE:
		return( v == 8 );
		}

	return( 0 );
}
#endif

#ifndef VAX
indexreg( p ) register NODE *p; 
{
	if( p->in.op == REG && p->tn.rval >= A0 && p->tn.rval <= SP ) return(1);
	return(0);
}
#endif

shumul( p ) register NODE *p; 
{
	register o;
	extern int xdebug;

	if (xdebug) {
		 printf("\nshumul:op=%d,lop=%d,rop=%d", p->in.op, p->in.left->in.op, p->in.right->in.op);
		printf(" prname=%s,plty=%d, prlval=%D\n", p->in.right->in.name, p->in.left->in.type, p->in.right->tn.lval);
		}


	o = p->in.op;
#ifdef VAX
	if( o == NAME || (o == OREG && !R2TEST(p->tn.rval)) || o == ICON ) return( STARNM );

	if( ( o == INCR || o == ASG MINUS ) &&
	    ( p->in.left->in.op == REG && p->in.right->in.op == ICON ) &&
	    p->in.right->in.name[0] == '\0' )
		{
		switch (p->in.left->in.type)
			{
			case CHAR|PTR:
			case UCHAR|PTR:
				o = 1;
				break;

			case SHORT|PTR:
			case USHORT|PTR:
				o = 2;
				break;

			case INT|PTR:
			case UNSIGNED|PTR:
			case LONG|PTR:
			case ULONG|PTR:
			case FLOAT|PTR:
				o = 4;
				break;

			case DOUBLE|PTR:
				o = 8;
				break;

			default:
				if ( ISPTR(p->in.left->in.type) ) {
					o = 4;
					break;
					}
				else return(0);
			}
		return( p->in.right->tn.lval == o ? STARREG : 0);
		}
#else
	if( indexreg(p) ) return( STARNM );

	if( o == INCR && indexreg(p->in.left) && p->in.right->in.op == ICON &&
	    p->in.right->in.name[0] == '\0' &&
	    spsz( p->in.left->in.type, p->in.right->tn.lval ) )
		return( STARREG );
#endif

	return( 0 );
}

adrcon( val ) CONSZ val; 
{
#ifdef VAX
	printf( "$" );
#endif
	printf( CONFMT, val );
}

conput( p ) register NODE *p; 
{
	switch( p->in.op ){

	case ICON:
		acon( p );
		return;

	case REG:
		printf( "%s", rnames[p->tn.rval] );
#ifndef VAX
		usedregs |= 1<<p->tn.rval;
#endif
		return;

	default:
		cerror( "illegal conput" );
		}
}

insput( p ) register NODE *p; 
{
	cerror( "insput" );
}

upput( p ) register NODE *p; 
{
#ifdef VAX
	cerror( "upput" );
#else
	/* output the address of the second word in the
	   pair pointed to by p (for LONGs)*/
	CONSZ save;

	if( p->in.op == FLD ){
		p = p->in.left;
		}

	save = p->tn.lval;
	switch( p->in.op ){

	case NAME:
		p->tn.lval += SZINT/SZCHAR;
		acon( p );
		break;

	case ICON:
		/* addressable value of the constant */
		p->tn.lval &= BITMASK(SZINT);
		printf( "#" );
		acon( p );
		break;

	case REG:
		printf( "%s", rnames[p->tn.rval+1] );
		usedregs |= 1<< (p->tn.rval+1);
		break;

	case OREG:
		p->tn.lval += SZINT/SZCHAR;
		if( p->tn.rval == A6 ){  /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			}
		printf( "%s@", rnames[p->tn.rval] );
		usedregs |= 1<<p->tn.rval;
		if( p->tn.lval != 0 || p->in.name[0] != '\0' )
		  { printf("("); acon( p ); printf(")"); }
		break;

	default:
		cerror( "illegal upper address" );
		break;

		}
	p->tn.lval = save;
#endif
}

adrput( p ) register NODE *p; 
{
	/* the 68k code saves lval and restores after the switch */
	register int r;
	/* output an address, with offsets, from p */

	if( p->in.op == FLD ){
		p = p->in.left;
		}
	switch( p->in.op ){

	case NAME:
		acon( p );
		return;

	case ICON:
		/* addressable value of the constant */
#ifdef VAX
		printf( "$" );
#else
		printf("#");
#endif
		acon( p );
		return;

	case REG:
		printf( "%s", rnames[p->tn.rval] );
#ifndef VAX
		usedregs |= 1<<p->tn.rval;
#endif
		return;

	case OREG:
#ifdef VAX
		r = p->tn.rval;
		if( R2TEST(r) ){ /* double indexing */
			register int flags;

			flags = R2UPK3(r);
			if( flags & 1 ) printf("*");
			if( flags & 4 ) printf("-");
			if( p->tn.lval != 0 || p->in.name[0] != '\0' ) acon(p);
			if( R2UPK1(r) != 100) printf( "(%s)", rnames[R2UPK1(r)] );
			if( flags & 2 ) printf("+");
			printf( "[%s]", rnames[R2UPK2(r)] );
			return;
			}
		if( r == AP ){  /* in the argument region */
			if( p->tn.lval <= 0 || p->in.name[0] != '\0' ) werror( "bad arg temp" );
			printf( CONFMT, p->tn.lval );
			printf( "(ap)" );
			return;
			}
		if( p->tn.lval != 0 || p->in.name[0] != '\0') acon( p );
		printf( "(%s)", rnames[p->tn.rval] );
		return;
#else
		if( p->tn.rval == A6 ) { /* in the argument region */
			if( p->in.name[0] != '\0' ) werror( "bad arg temp" );
			printf( "a6@(" );
			printf( CONFMT, p->tn.lval );
			printf( ")" );
			return;
			}
		printf( "%s@", rnames[p->tn.rval] );
		usedregs |= 1<<p->tn.rval;
		if( p->tn.lval != 0 || p->in.name[0] != '\0' )
		  { printf("("); acon( p ); printf(")"); }
		return;
#endif

	case UNARY MUL:
		/* STARNM or STARREG found */
#ifdef VAX
		if( tshape(p, STARNM) ) {
			printf( "*" );
			adrput( p->in.left);
			}
		else {	/* STARREG - really auto inc or dec */
			register NODE *q;

/* tbl
			p = p->in.left;
			p->in.left->in.op = OREG;
			if( p->in.op == INCR ) {
				adrput( p->in.left );
				printf( "+" );
				}
			else {
				printf( "-" );
				adrput( p->in.left );
				}
   tbl */
			printf("%s(%s)%s", (p->in.left->in.op==INCR ? "" : "-"),
				rnames[p->in.left->in.left->tn.rval], 
				(p->in.left->in.op==INCR ? "+" : "") );
			p->in.op = OREG;
			p->tn.rval = p->in.left->in.left->tn.rval;
			q = p->in.left;
			p->tn.lval = (p->in.left->in.op == INCR ? -p->in.left->in.right->tn.lval : 0);
#ifndef FLEXNAMES
			p->in.name[0] = '\0';
#else
			p->in.name = "";
#endif
			tfree(q);
		}
		return;

#else
		if( tshape(p, STARNM) ) {
			adrput( p->in.left);
			printf( "@" );
			}
		else {	/* STARREG - really auto inc or dec */
			/* turn into OREG so replacement node will
			   reflect the value of the expression */
			register i;
			register NODE *q, *l;

			l = p->in.left;
			q = l->in.left;
			p->in.op = OREG;
			p->in.rall = q->in.rall;
			p->tn.lval = q->tn.lval;
			p->tn.rval = q->tn.rval;
#ifndef FLEXNAMES
			for( i=0; i<NCHNAM; i++ )
				p->in.name[i] = q->in.name[i];
#else
			p->in.name = q->in.name;
#endif
			if( l->in.op == INCR ) {
				adrput( p );
				printf( "+" );
				p->tn.lval -= l->in.right->tn.lval;
				}
			else {	/* l->in.op == ASG MINUS */
				printf( "-" );
				adrput( p );
				}
			tfree( l );
		}
		return;

#endif

	default:
		cerror( "illegal address" );
		return;

	}

}

acon( p ) register NODE *p; 
{ /* print out a constant */

	if( p->in.name[0] == '\0' ){
		printf( CONFMT, p->tn.lval);
		}
	else if( p->tn.lval == 0 ) {
#ifndef FLEXNAMES
		printf( "%.8s", p->in.name );
#else
		printf( "%s", p->in.name );
#endif
		}
	else {
#ifndef FLEXNAMES
		printf( "%.8s+", p->in.name );
#else
		printf( "%s+", p->in.name );
#endif
		printf( CONFMT, p->tn.lval );
		}
}

/*
aacon( p ) register NODE *p; 
{ /* print out a constant */
/*

	if( p->in.name[0] == '\0' ){
		printf( CONFMT, p->tn.lval);
		return( 0 );
		}
	else if( p->tn.lval == 0 ) {
#ifndef FLEXNAMES
		printf( "$%.8s", p->in.name );
#else
		printf( "$%s", p->in.name );
#endif
		return( 1 );
		}
	else {
		printf( "$(" );
		printf( CONFMT, p->tn.lval );
		printf( "+" );
#ifndef FLEXNAMES
		printf( "%.8s)", p->in.name );
#else
		printf( "%s)", p->in.name );
#endif
		return(1);
		}
}
 */

#ifdef VAX
genscall( p, cookie ) register NODE *p; 
{
	/* structure valued call */
	return( gencall( p, cookie ) );
}

/* tbl */
int gc_numbytes;
/* tbl */

gencall( p, cookie ) register NODE *p; 
{
	/* generate the call given by p */
	register NODE *p1, *ptemp;
	register temp, temp1;
	register m;

	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.op == STCALL || p->in.op == UNARY STCALL ){
		/* set aside room for structure return */

		if( p->stn.stsize > temp ) temp1 = p->stn.stsize;
		else temp1 = temp;
		}

	if( temp > maxargs ) maxargs = temp;
	SETOFF(temp1,4);

	if( p->in.right ){ /* make temp node, put offset in, and generate args */
		ptemp = talloc();
		ptemp->in.op = OREG;
		ptemp->tn.lval = -1;
		ptemp->tn.rval = SP;
#ifndef FLEXNAMES
		ptemp->in.name[0] = '\0';
#else
		ptemp->in.name = "";
#endif
		ptemp->in.rall = NOPREF;
		ptemp->in.su = 0;
		genargs( p->in.right, ptemp );
		ptemp->in.op = FREE;
		}

	p1 = p->in.left;
	if( p1->in.op != ICON ){
		if( p1->in.op != REG ){
			if( p1->in.op != OREG || R2TEST(p1->tn.rval) ){
				if( p1->in.op != NAME ){
					order( p1, INAREG );
					}
				}
			}
		}

/*
	if( p1->in.op == REG && p->tn.rval == R5 ){
		cerror( "call register overwrite" );
		}
 */
/* tbl
	setup gc_numbytes so reference to ZC works */

	gc_numbytes = temp&(0x3ff);
/* tbl */

	p->in.op = UNARY CALL;
	m = match( p, INTAREG|INTBREG );

	/* compensate for deficiency in 'ret' instruction ... wah,kre */
	/* (plus in assignment to gc_numbytes above, for neatness only) */
	if (temp >= 1024)
		printf("	addl2	$%d,sp\n", (temp&(~0x3ff)));

/* tbl
	switch( temp ) {
	case 0:
		break;
	case 2:
		printf( "	tst	(sp)+\n" );
		break;
	case 4:
		printf( "	cmp	(sp)+,(sp)+\n" );
		break;
	default:
		printf( "	add	$%d,sp\n", temp);
		}
   tbl */
	return(m != MDONE);
}

/* End of VAX gencall */
#else

genscall( p, cookie ) register NODE *p; 
{
	/* structure valued call */
	return( gencall( p, cookie ) );
}

gencall( p, cookie ) register NODE *p; 
{
	/* generate the call given by p */
	register temp;
	register m;

	if( p->in.right ) temp = argsize( p->in.right );
	else temp = 0;

	if( p->in.right ){ /* generate args */
		genargs( p->in.right );
		}

	if( !shltype( p->in.left->in.op, p->in.left ) ) {
		order( p->in.left, INBREG|SOREG );
		}

	p->in.op = UNARY CALL;
	m = match( p, INTAREG|INTBREG );
	popargs( temp );
	return(m != MDONE);
}

popargs( size ) register size; 
{
	/* pop arguments from stack */

	toff -= size/2;
	if( toff == 0 && size >= 2 ) size -= 2;
	switch( size ) {
	case 0:
		break;
	default:
		printf( "	%s	#%d,sp\n", size<=8?"addql":"addl",size);
		}
}

#endif

/* tbl */
char *
ccbranches[] = {
#ifdef VAX
	"	jeql	L%d\n",
	"	jneq	L%d\n",
	"	jleq	L%d\n",
	"	jlss	L%d\n",
	"	jgeq	L%d\n",
	"	jgtr	L%d\n",
	"	jlequ	L%d\n",
	"	jlssu	L%d\n",
	"	jgequ	L%d\n",
	"	jgtru	L%d\n",
#else
	"	beq	L%d\n",
	"	bne	L%d\n",
	"	ble	L%d\n",
	"	blt	L%d\n",
	"	bge	L%d\n",
	"	bgt	L%d\n",
	"	bls	L%d\n",
	"	bcs	L%d\n",		/* blo */
	"	bcc	L%d\n",		/* bhis */
	"	bhi	L%d\n",
#endif
	};

#ifdef VAX
/* tbl */

cbgen( o, lab, mode ) 
{ /*   printf conditional and unconditional branches */

/* tbl */
	if( o == 0 ) printf( "	jbr	L%d\n", lab );
/* tbl */
	else {
		if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
		printf( ccbranches[o-EQ], lab );
		}
}
#else
/*	long branch table

   This table, when indexed by a logical operator,
   selects a set of three logical conditions required
   to generate long comparisons and branches.  A zero
   entry indicates that no branch is required.
   E.G.:  The <= operator would generate:
	cmp	AL,AR
	jlt	lable	/ 1st entry LT -> lable
	jgt	1f	/ 2nd entry GT -> 1f
	cmp	UL,UR
	jlos	lable	/ 3rd entry ULE -> lable
   1:
 */

int lbranches[][3] = {
	/*EQ*/	0,	NE,	EQ,
	/*NE*/	NE,	0,	NE,
	/*LE*/	LT,	GT,	ULE,
	/*LT*/	LT,	GT,	ULT,
	/*GE*/	GT,	LT,	UGE,
	/*GT*/	GT,	LT,	UGT,
	/*ULE*/	ULT,	UGT,	ULE,
	/*ULT*/	ULT,	UGT,	ULT,
	/*UGE*/	UGT,	ULT,	UGE,
	/*UGT*/	UGT,	ULT,	UGT,
	};

/* logical relations when compared in reverse order (cmp R,L) */
#ifndef VAX
#ifdef FORT
short revrel[] ={ EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT };
#else
extern short revrel[] ;
#endif
#endif

cbgen( o, lab, mode ) 
{ /*   printf conditional and unconditional branches */
	register *plb;
	int lab1f;

	if( o == 0 ) printf( "	bra	L%d\n", lab );
	else	if( o > UGT ) cerror( "bad conditional branch: %s", opst[o] );
	else {
		switch( brcase ) {

		case 'A':
		case 'C':
			plb = lbranches[ o-EQ ];
			lab1f = getlab();
			expand( brnode, FORCC, brcase=='C' ? "\tcmp\tAL,AR\n" : "\ttst\tAR\n" );
			if( *plb != 0 )
				printf( ccbranches[*plb-EQ], lab);
			if( *++plb != 0 )
				printf( ccbranches[*plb-EQ], lab1f);
			expand( brnode, FORCC, brcase=='C' ? "\tcmp\tUL,UR\n" : "\ttst\tUR\n" );
			printf( ccbranches[*++plb-EQ], lab);
			deflab( lab1f );
			reclaim( brnode, RNULL, 0 );
			break;

		default:
			if( mode=='F' ) o = revrel[ o-EQ ];
			printf( ccbranches[o-EQ], lab );
			break;
			}

		brcase = 0;
		brnode = 0;
		}
}
#endif

nextcook( p, cookie ) NODE *p; 
{
	/* we have failed to match p with cookie; try another */
	if( cookie == FORREW ) return( 0 );  /* hopeless! */
	if( !(cookie&(INTAREG|INTBREG)) ) return( INTAREG|INTBREG );
	if( !(cookie&INTEMP) && asgop(p->in.op) ) return( INTEMP|INAREG|INTAREG|INTBREG|INBREG );
	return( FORREW );
}

lastchance( p, cook ) NODE *p; 
{
	/* forget it! */
	return(0);
}

optim2( p ) register NODE *p; 
{
#ifdef VAX
	/* do local tree transformations and optimizations */

	register NODE *r;

	switch( p->in.op ) {

	case AND:
		/* commute L and R to eliminate compliments and constants */
		if( (p->in.left->in.op==ICON&&p->in.left->in.name[0]==0) || p->in.left->in.op==COMPL ) {
			r = p->in.left;
			p->in.left = p->in.right;
			p->in.right = r;
			}
	case ASG AND:
		/* change meaning of AND to ~R&L - bic on pdp11 */
		r = p->in.right;
		if( r->in.op==ICON && r->in.name[0]==0 ) { /* compliment constant */
			r->tn.lval = ~r->tn.lval;
			}
		else if( r->in.op==COMPL ) { /* ~~A => A */
			r->in.op = FREE;
			p->in.right = r->in.left;
			}
		else { /* insert complement node */
			p->in.right = talloc();
			p->in.right->in.op = COMPL;
			p->in.right->in.rall = NOPREF;
			p->in.right->in.type = r->in.type;
			p->in.right->in.left = r;
			p->in.right->in.right = NULL;
			}
		break;

		}
#else
  register NODE *q;

	/* multiply of two shorts to produce an int can be done directly
	 * in the hardware.
	 */
	if ( p->in.op==MUL && p->in.type==INT && shortconv(p->in.left) &&
	     (shortconv(p->in.right) || (p->in.right->in.op==ICON &&
	      p->in.right->in.name[0] == '\0' &&
	      p->in.right->tn.lval>=-32768L && p->in.right->tn.lval<=32767L)))
	{
	  p->in.type = SHORT;
	  p->in.left = (q = p->in.left)->in.left;
	  q->in.op = FREE;
	  if ( p->in.right->in.op==ICON ) p->in.right->in.type = SHORT;
	  else {
	    p->in.right = (q = p->in.right)->in.left;
	    q->in.op = FREE;
	  }
	}

	/* change <flt exp>1 <logop> <flt exp>2 to
	 * (<exp>1 - <exp>2) <logop> 0.0
	 */
	if (logop(p->in.op) &&
	    ((q = p->in.left)->in.type==FLOAT || q->in.type==DOUBLE) &&
	    ((q = p->in.right)->in.type==FLOAT || q->in.type==DOUBLE)) {
	  q = talloc();
	  q->in.op = MINUS;
	  q->in.rall = NOPREF;
	  q->in.type = DOUBLE;
	  q->in.left = p->in.left;
	  q->in.right = p->in.right;
	  p->in.left = q;
	  p->in.right = q = talloc();
	  q->tn.op = ICON;
	  q->tn.type = DOUBLE;
#ifndef FLEXNAMES
	  q->tn.name[0] = '\0';
#else
	  q->tn.name = "";
#endif
	  q->tn.rval = 0;
	  q->tn.lval = 0;
	}
#endif
}

NODE * addroreg(l)
				/* OREG was built in clocal()
				 * for an auto or formal parameter
				 * now its address is being taken
				 * local code must unwind it
				 * back to PLUS/MINUS REG ICON
				 * according to local conventions
				 */
{
	cerror("address of OREG taken");
}



# ifndef ONEPASS
main( argc, argv ) char *argv[]; 
{
	return( mainp2( argc, argv ) );
}
# endif


#ifdef VAX
/* added by jwf */
struct functbl {
	int fop;
	TWORD ftype;
	char *func;
	} opfunc[] = {
	DIV,		TANY,	"udiv",
	MOD,		TANY,	"urem",
	ASG DIV,	TANY,	"udiv",
	ASG MOD,	TANY,	"urem",
	0,	0,	0 };

hardops(p)  register NODE *p; 
{
	/* change hard to do operators into function calls.  */
	register NODE *q;
	register struct functbl *f;
	register o;
	register TWORD t;

	o = p->in.op;
	t = p->in.type;
	if( t!=UNSIGNED && t!=ULONG ) return;

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop ) goto convert;
		}
	return;

	/* need to rewrite tree for ASG OP */
	/* must change ASG OP to a simple OP */
	convert:
	if( asgop( o ) ) {
		q = talloc();
		switch( p->in.op ) {
			case ASG DIV:
				q->in.op = DIV;
				break;
			case ASG MOD:
				q->in.op = MOD;
				break;
		}
		q->in.rall = NOPREF;
		q->in.type = p->in.type;
		q->in.left = tcopy(p->in.left);
		q->in.right = p->in.right;
		p->in.op = ASSIGN;
		p->in.right = q;
		zappost(q->in.left); /* remove post-INCR(DECR) from new node */
		fixpre(q->in.left);	/* change pre-INCR(DECR) to +/-	*/
		p = q;

	}

	/* build comma op for args to function */
	q = talloc();
	q->in.op = CM;
	q->in.rall = NOPREF;
	q->in.type = INT;
	q->in.left = p->in.left;
	q->in.right = p->in.right;
	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
#ifndef FLEXNAMES
	strcpy( q->in.name, f->func );
#else
	q->in.name = f->func;
#endif
	q->tn.lval = 0;
	q->tn.rval = 0;

	return;

}

zappost(p) NODE *p; 
{
	/* look for ++ and -- operators and remove them */

	register o, ty;
	register NODE *q;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case INCR:
	case DECR:
			q = p->in.left;
			p->in.right->in.op = FREE;  /* zap constant */
			ncopy( p, q );
			q->in.op = FREE;
			return;

		}

	if( ty == BITYPE ) zappost( p->in.right );
	if( ty != LTYPE ) zappost( p->in.left );
}

fixpre(p) NODE *p; 
{

	register o, ty;
	o = p->in.op;
	ty = optype( o );

	switch( o ){

	case ASG PLUS:
			p->in.op = PLUS;
			break;
	case ASG MINUS:
			p->in.op = MINUS;
			break;
		}

	if( ty == BITYPE ) fixpre( p->in.right );
	if( ty != LTYPE ) fixpre( p->in.left );
}
#else
struct functbl {
	int fop;
	TWORD ftype;
	char *func;
	} opfunc[] = {
	MUL,		INT,	"lmul",
	DIV,		INT,	"ldiv",
	MOD,		INT,	"lrem",
	ASG MUL,	INT,	"almul",
	ASG DIV,	INT,	"aldiv",
	ASG MOD,	INT,	"alrem",
	MUL,		UNSIGNED,	"ulmul",
	DIV,		UNSIGNED,	"uldiv",
	MOD,		UNSIGNED,	"ulrem",
	ASG MUL,	UNSIGNED,	"aulmul",
	ASG DIV,	UNSIGNED,	"auldiv",
	ASG MOD,	UNSIGNED,	"aulrem",
	PLUS,		DOUBLE,	"fadd",
	MINUS,		DOUBLE, "fsub",
	MUL,		DOUBLE, "fmul",
	DIV,		DOUBLE, "fdiv",
	UNARY MINUS,	DOUBLE, "fneg",
	UNARY MINUS,	FLOAT,	"fneg",
	ASG PLUS,	DOUBLE,	"afadd",
	ASG MINUS,	DOUBLE, "afsub",
	ASG MUL,	DOUBLE, "afmul",
	ASG DIV,	DOUBLE, "afdiv",
	ASG PLUS,	FLOAT,	"afaddf",
	ASG MINUS,	FLOAT,	"afsubf",
	ASG MUL,	FLOAT,	"afmulf",
	ASG DIV,	FLOAT,	"afdivf",
	0,	0,	0 };

hardops(p)  register NODE *p; 
{
	/* change hard to do operators into function calls. */
	register NODE *q;
	register struct functbl *f;
	register o;
	register TWORD t;

	o = p->in.op;
	t = p->in.type;

	if (o==SCONV) { hardconv(p); return; }

	for( f=opfunc; f->fop; f++ ) {
		if( o==f->fop && t==f->ftype ) goto convert;
		}
	return;

	/* need address of left node for ASG OP */
	/* WARNING - this won't work for long in a REG */
	convert:
	if( asgop( o ) ) {
		switch( p->in.left->in.op ) {

		case UNARY MUL:	/* convert to address */
			p->in.left->in.op = FREE;
			p->in.left = p->in.left->in.left;
			break;

		case NAME:	/* convert to ICON pointer */
			p->in.left->in.op = ICON;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			break;

		case OREG:	/* convert OREG to address */
			p->in.left->in.op = REG;
			p->in.left->in.type = INCREF( p->in.left->in.type );
			if( p->in.left->tn.lval != 0 ) {
				q = talloc();
				q->in.op = PLUS;
				q->in.rall = NOPREF;
				q->in.type = p->in.left->in.type;
				q->in.left = p->in.left;
				q->in.right = talloc();

				q->in.right->in.op = ICON;
				q->in.right->in.rall = NOPREF;
				q->in.right->in.type = INT;
				q->in.right->in.name[0] = '\0';
				q->in.right->tn.lval = p->in.left->tn.lval;
				q->in.right->tn.rval = 0;

				p->in.left->tn.lval = 0;
				p->in.left = q;
				}
			break;

		/* rewrite "foo <op>= bar" as "foo = foo <op> bar" for foo in a reg */
		case REG:
			q = talloc();
			q->in.op = p->in.op - 1;	/* change <op>= to <op> */
			q->in.rall = p->in.rall;
			q->in.type = p->in.type;
			q->in.left = talloc();
			q->in.right = p->in.right;
			p->in.op = ASSIGN;
			p->in.right = q;
			q = q->in.left;			/* make a copy of "foo" */
			q->in.op = p->in.left->in.op;
			q->in.rall = p->in.left->in.rall;
			q->in.type = p->in.left->in.type;
			q->tn.lval = p->in.left->tn.lval;
			q->tn.rval = p->in.left->tn.rval;
			hardops(p->in.right);
			return;

		default:
			cerror( "Bad address for hard ops" );
			/* NO RETURN */

			}
		}

	/* build comma op for args to function */
	if ( optype(p->in.op) == BITYPE ) {
	  q = talloc();
	  q->in.op = CM;
	  q->in.rall = NOPREF;
	  q->in.type = INT;
	  q->in.left = p->in.left;
	  q->in.right = p->in.right;
	} else q = p->in.left;

	p->in.op = CALL;
	p->in.right = q;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
#ifndef FLEXNAMES
	strcpy( q->in.name, f->func );
#else
	q->in.name = f->func;
#endif
	q->tn.lval = 0;
	q->tn.rval = 0;

	return;

}

/* do fix and float conversions */
hardconv(p)
register NODE *p;
{
	register NODE *q;
	register TWORD t,tl;
	int m,ml;

	t = p->in.type;
	tl = p->in.left->in.type;

	m = t==DOUBLE || t==FLOAT;
	ml = tl==DOUBLE || tl==FLOAT;

	if (m==ml) return;

	p->in.op = CALL;
	p->in.right = p->in.left;

	/* put function name in left node of call */
	p->in.left = q = talloc();
	q->in.op = ICON;
	q->in.rall = NOPREF;
	q->in.type = INCREF( FTN + p->in.type );
#ifndef FLEXNAMES
	strcpy( q->tn.name, m ? "float" : "fix" );
#else
	q->tn.name = m ? "float" : "fix" ;
#endif
	q->tn.lval = 0;
	q->tn.rval = 0;
}

/* return 1 if node is a SCONV from short to int */
shortconv( p )
  register NODE *p;
  {	if ( p->in.op==SCONV && p->in.type==INT && p->in.left->in.type==SHORT)
	  return( 1 );
	return( 0 );
}


#endif

myreader(p) register NODE *p; 
{
#ifdef VAX
	walkf( p, hardops );	/* convert ops to function calls */
	canon( p );		/* expands r-vals for fileds */
	walkf( p, optim2 );
	/* jwf toff = 0;  /* stack offset swindle */
#else
	walkf( p, optim2 );
	walkf( p, hardops );	/* convert ops to function calls */
	canon( p );		/* expands r-vals for fileds */
	toff = 0;  /* stack offset swindle */
#endif
}


#ifndef VAX
special( p, shape ) register NODE *p; 
{
	/* special shape matching routine */

	switch( shape ) {

	case SCCON:
		if( p->in.op == ICON && p->in.name[0]=='\0' && p->tn.lval>= -128 && p->tn.lval <=127 ) return( 1 );
		break;

	case SICON:
		if( p->in.op == ICON && p->in.name[0]=='\0' && p->tn.lval>= 0 && p->tn.lval <=32767 ) return( 1 );
		break;

	case S8CON:
		if( p->in.op == ICON && p->in.name[0]=='\0' && p->tn.lval>= 1 && p->tn.lval <=8 ) return( 1 );
		break;

	default:
		cerror( "bad special shape" );

		}

	return( 0 );
}
#endif
#ifndef VAX
# ifdef MULTILEVEL
# include "mldec.h"

struct ml_node mltree[] ={

DEFINCDEC,	INCR,	0,
	INCR,	SANY,	TANY,
		OPANY,	SAREG|STAREG,	TANY,
		OPANY,	SCON,	TANY,

DEFINCDEC,	ASG MINUS,	0,
	ASG MINUS,	SANY,	TANY,
		REG,	SANY,	TANY,
		ICON,	SANY,	TANY,

TSOREG,	1,	0,
	UNARY MUL,	SANY,	TANY,
		REG,	SANY,	TANY,

TSOREG,	2,	0,
	UNARY MUL,	SANY,	TANY,
		PLUS,	SANY,	TANY,
			REG,	SANY,	TANY,
			ICON,	SANY,	TCHAR|TUCHAR|TSHORT|TUSHORT|TINT|TUNSIGNED|TPOINT,

TSOREG,	2,	0,
	UNARY MUL,	SANY,	TANY,
		MINUS,	SANY,	TANY,
			REG,	SANY,	TANY,
			ICON,	SANY,	TCHAR|TUCHAR|TSHORT|TUSHORT|TINT|TUNSIGNED|TPOINT,
0,0,0};
# endif
#endif
