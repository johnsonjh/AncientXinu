static char *sccsid ="%W% (Berkeley) %G%";
# include "mfile1"
# include <sys/types.h>
# include <a.out.h>
# include <stab.h>

#ifndef VAX
extern int usedregs;
extern char *rnames[];
#endif
int proflg = 0;	/* are we generating profiling code? */
int strftn = 0;  /* is the current function one which returns a value */
int fltfun = 0; /* is function float or double? */
int gdebug;
int fdefflag;  /* are we within a function definition ? */
char NULLNAME[8];
int labelno;

branch( n )
{
	/* output a branch to label n */
	/* exception is an ordinary function branching to retlab: then, return */
	if( n == retlab && !strftn ){
#ifdef VAX
		printf( "	ret\n" );
#else
		printf( "	bra	LE%d\n", ftnno );
#endif
		}
	else 
#ifdef VAX
		printf( "	jbr 	L%d\n", n );
#else
		printf("	bra		L%d\n", n);
#endif
}

int lastloc = { -1 };

#ifdef VAX
short log2tab[] = {0, 0, 1, 2, 2, 3, 3, 3, 3};
#define LOG2SZ 9
#endif

defalign(n) 
{
	/* cause the alignment to become a multiple of n */
#ifdef VAX
	n /= SZCHAR;
	if( lastloc != PROG && n > 1 ) printf( "	.align	%d\n", n >= 0 && n < LOG2SZ ? log2tab[n] : 0 );
#else
	if ( lastloc != PROG && n > 1 ) printf("	.even\n");
#endif
}

locctr( l )
{
	register temp;
	/* l is PROG, ADATA, DATA, STRNG, ISTRNG, or STAB */

	if( l == lastloc ) return(l);
	temp = lastloc;
	lastloc = l;
	switch( l ){

	case PROG:
		printf( "	.text\n" );
		psline();
		break;

	case DATA:
	case ADATA:
		if (temp != DATA && temp != ADATA)
			printf( "	.data\n" );
		break;

	/* The following levels of .data statements will be problems if
	   there is ever any relocation required. */
	case STRNG:
#ifdef VAX
		printf( "	.data	1\n" );
#else
		printf( "	.data1\n" );
#endif
		break;

	case ISTRNG:
#ifdef VAX
		printf( "	.data	2\n" );
#else
		printf( "	.data2\n" );
#endif
		break;

	case STAB:
		printf( "	.stab\n" );
		break;

	default:
		cerror( "illegal location counter" );
		}

	return( temp );
}

deflab( n )
{
	/* output something to define the current position as label n */
	printf( "L%d:\n", n );
}

int crslab = 10;

getlab()
{
	/* return a number usable for a label */
	return( ++crslab );
}


#ifdef VAX
int ent_mask[] = {
	0,0,0,0,0, 0xfc0, 0xf80, 0xf00, 0xe00, 0xc00, 0x800, 0};

int reg_use = 11;
#else
extern int regsused;
#endif

efcode()
{
	/* code for the end of a function */

#ifdef VAX
	if( strftn ){  /* copy output (in R2) to caller */
		register NODE *l, *r;
		register struct symtab *p;
		register TWORD t;
		register int j;
		int i;

		p = &stab[curftn];
		t = p->stype;
		t = DECREF(t);

		deflab( retlab );

		i = getlab();	/* label for return area */
#ifndef LCOMM
		printf("	.data\n" );
		printf("	.align	2\n" );
		printf("L%d:	.space	%d\n", i, tsize(t, p->dimoff, p->sizoff)/SZCHAR );
		printf("	.text\n" );
#else
		{ int sz = tsize(t, p->dimoff, p->sizoff) / SZCHAR;
		if (sz % sizeof (int))
			sz += sizeof (int) - (sz % sizeof (int));
		printf("	.lcomm	L%d,%d\n", i, sz);
		}
#endif
		psline();
		printf("	movab	L%d,r1\n", i);

		reached = 1;
		l = block( REG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		l->tn.rval = 1;  /* R1 */
		l->tn.lval = 0;  /* no offset */
		r = block( REG, NIL, NIL, PTR|t, p->dimoff, p->sizoff );
		r->tn.rval = 0;  /* R0 */
		r->tn.lval = 0;
		l = buildtree( UNARY MUL, l, NIL );
		r = buildtree( UNARY MUL, r, NIL );
		l = buildtree( ASSIGN, l, r );
		l->in.op = FREE;
		ecomp( l->in.left );
		printf( "	movab	L%d,r0\n", i );
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
		}
	branch( retlab );
#ifndef VMS
	printf( "	.set	L%d,0x%x\n", ftnno, ent_mask[reg_use] );
#else
	printf( "	.set	L%d,%d	# Hex = 0x%x\n", ftnno, 0x3c| ent_mask[reg_use], ent_mask[reg_use]  );
	/* KLS kludge, under VMS if you use regs 2-5, you must save them. */
#endif
	reg_use = 11;
	p2bend();
	fdefflag = 0;
#else
/* 68k code */
/* NOTE: We still have to add FTN and stab support here */
	if( strftn ){  /* copy output (in r0) to caller */
		register struct symtab *p;
		register int stlab;
		register int count;
		int size;

		p = &stab[curftn];

		deflab( retlab );

		stlab = getlab();
		printf( "	movl	d0,a0\n" );
		printf( "	movl	#L%d,a1\n", stlab );
		size = tsize( DECREF(p->stype), p->dimoff, p->sizoff ) / SZCHAR;
		count = size/2;
		while( size ){ /* simple load/store loop */
		  count = (size > 2) ? 4 : 2;
		  printf("	mov%c	a0@+,a1@+\n", count==2 ? 'w' : 'l');
		  size -= count;
		}
		printf( "	movl	#L%d,d0\n", stlab );
		printf( "	.bss\nL%d:	.=.+%d\n	.text\n", stlab, size );
		/* turn off strftn flag, so return sequence will be generated */
		strftn = 0;
		}
	branch( retlab );
	p2bend();
#endif
}

int ftlab1, ftlab2;

bfcode( a, n ) int a[]; 
{
	/* code for the beginning of a function; a is an array of
		indices in stab for the arguments; n is the number */
	register i;
	register temp;
	register struct symtab *p;
	int off;
	char *toreg();

	locctr( PROG );
	p = &stab[curftn];
#ifdef VAX
	printf( "	.align	1\n");
#endif
	defnam( p );
	temp = p->stype;
	temp = DECREF(temp);
	strftn = (temp==STRTY) || (temp==UNIONTY);
	fltfun = (temp==FLOAT) || (temp==DOUBLE);

	retlab = getlab();

	/* routine prolog */

#ifdef VAX
	printf( "	.word	L%d\n", ftnno);
#else
	printf("	link	a6,#0\n");
	printf("	addl	#-LF%d,sp\n", ftnno);
	printf("	moveml	#LS%d,sp@\n",ftnno);
#endif

	if (gdebug) {
#ifdef STABDOT
		pstabdot(N_SLINE, lineno);
#else
		pstab(NULLNAME, N_SLINE);
		printf("0,%d,LL%d\n", lineno, labelno);
		printf("LL%d:\n", labelno++);
#endif
	}
	ftlab1 = getlab();
	ftlab2 = getlab();
#ifdef VAX
	printf( "	jbr 	L%d\n", ftlab1);
#else
	printf( "	bra 	L%d\n", ftlab1);
#endif
	printf( "L%d:\n", ftlab2);
	if( proflg ) {	/* profile code */
		i = getlab();
#ifdef VAX
		printf("	movab	L%d,r0\n", i);
		printf("	jsb 	mcount\n");
		printf("	.data\n");
		printf("	.align	2\n");
		printf("L%d:	.long	0\n", i);
		printf("	.text\n");
#else
		printf("	movel	#L%d,a0\n",i);
		printf("	jbsr	mcount\n");
		printf("	.bss\nL%d:	.=.+2\n	.text\n",i);
#endif
		psline();
		}

	off = ARGINIT;

	for( i=0; i<n; ++i ){
#ifndef VAX
		char type;
#endif

		p = &stab[a[i]];
		if( p->sclass == REGISTER ){
			temp = p->offset;  /* save register number */
			p->sclass = PARAM;  /* forget that it is a register */
			p->offset = NOOFFSET;
			oalloc( p, &off );
#ifdef VAX
/*tbl*/		printf( "	%s	%d(ap),r%d\n", toreg(p->stype), p->offset/SZCHAR, temp );
#else
			printf( "	%s	a6@(%d),%s\n", toreg(p->stype), p->offset/SZCHAR, rnames[temp]);
			usedregs |= 1<<temp;
#endif
			p->offset = temp;  /* remember register number */
			p->sclass = REGISTER;   /* remember that it is a register */
			}
		/* The 68k doesn't have this code.  I'll try it for a while */
#ifdef VAX
		else if( p->stype == STRTY || p->stype == UNIONTY ) {
			p->offset = NOOFFSET;
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			SETOFF( off, ALSTACK );
			}
#endif
		else {
			if( oalloc( p, &off ) ) cerror( "bad argument" );
			}

		}
	fdefflag = 1;
}

bccode()
{ /* called just before the first executable statment */
		/* by now, the automatics and register variables are allocated */
	SETOFF( autooff, SZINT );
	/* set aside store area offset */
	p2bbeg( autooff, regvar );
#ifdef VAX
	reg_use = (reg_use > regvar ? regvar : reg_use);
#endif
}

ejobcode( flag )
{
	/* called just before final exit */
	/* flag is 1 if errors, 0 if none */
}

aobeg()
{
	/* called before removing automatics from stab */
}

aocode(p) struct symtab *p; 
{
	/* called when automatic p removed from stab */
}

aoend()
{
	/* called after removing all automatics from stab */
}

defnam( p ) register struct symtab *p; 
{
	/* define the current location as the name p->sname */

	if( p->sclass == EXTDEF ){
		printf( "	.globl	%s\n", exname( p->sname ) );
		}
	if( p->sclass == STATIC && p->slevel>1 ) deflab( p->offset );
	else printf( "%s:\n", exname( p->sname ) );

}

bycode( t, i )
{
#ifdef ASSTRINGS
static	int	lastoctal = 0;
#endif

	/* put byte i+1 in a string */

#ifdef ASSTRINGS

	i &= 077;
	if ( t < 0 ){
		if ( i != 0 )	printf( "\"\n" );
	} else {
		if ( i == 0 ) printf("\t.ascii\t\"");
		if ( t == '\\' || t == '"'){
			lastoctal = 0;
			printf("\\%c", t);
		}
			/*
			 *	We escape the colon in strings so that
			 *	c2 will, in its infinite wisdom, interpret
			 *	the characters preceding the colon as a label.
			 *	If we didn't escape the colon, c2 would
			 *	throw away any trailing blanks or tabs after
			 *	the colon, but reconstruct a assembly
			 *	language semantically correct program.
			 *	C2 hasn't been taught about strings.
			 */
		else if ( t == ':' || t < 040 || t >= 0177 || t == '|' || t == ';' ){
			lastoctal++;
			printf("\\%o",t);
		}
		else if ( lastoctal && '0' <= t && t <= '9' ){
			lastoctal = 0;
			printf("\"\n\t.ascii\t\"%c", t );
		}
		else
		{	
			lastoctal = 0;
			putchar(t);
		}
		if ( i == 077 ) printf("\"\n");
	}
#else

	i &= 07;
	if( t < 0 ){ /* end of the string */
		if( i != 0 ) printf( "\n" );
		}

	else { /* stash byte t into string */
		if( i == 0 ) printf( "	.byte	" );
		else printf( "," );
#ifdef VAX
		printf( "0x%x", t );
#else
		printf( "0x%x", t );
#endif
		if( i == 07 ) printf( "\n" );
		}
#endif
}

zecode( n )
{
	/* n integer words of zeros */
	OFFSZ temp;
#ifndef VAX
	register i;
#endif

	if( n <= 0 ) return;
#ifdef VAX
	printf( "	.space	%d\n", (SZINT/SZCHAR)*n );
#else
	for(i=0;i<n;i++) printf("	.long	0\n");
#endif
	temp = n;
	inoff += temp*SZINT;
}

fldal( t ) unsigned t; 
{ /* return the alignment of field of type t */
	uerror( "illegal field type" );
	return( ALINT );
}

fldty( p ) struct symtab *p; 
{ /* fix up type of field p */
	;
}


where(c)
{ /* print location of error  */
	/* c is either 'u', 'c', or 'w' */
	/* GCOS version */
	fprintf( stderr, "%s, line %d: ", ftitle, lineno );
}


/* tbl - toreg() returns a pointer to a char string
		  which is the correct  "register move" for the passed type 
 */
struct type_move 
{TWORD fromtype; char tostrng[8];} toreg_strs[] =
	{
#ifdef VAX
	CHAR, "cvtbl",
	SHORT, "cvtwl",
	INT, "movl",
	LONG, "movl",
	FLOAT, "movf",
	DOUBLE, "movd",
	UCHAR,	"movzbl",
	USHORT,	"movzwl",
	UNSIGNED,	"movl",
	ULONG,	"movl",
#else
	CHAR, "movb",
	UCHAR, "movb",
	SHORT, "movw",
	USHORT, "movw",
#endif
	-1, ""
	};

char
*toreg(type)
	TWORD type;
{
	struct type_move *p;

	for ( p=toreg_strs; p->fromtype > 0; p++)
		if (p->fromtype == type) return(p->tostrng);

	/* type not found, must be a pointer type */
	return("movl");
}
/* tbl */


main( argc, argv ) char *argv[]; 
{
#ifdef BUFSTDERR
	char errbuf[BUFSIZ];
	setbuf(stderr, errbuf);
#endif
	return(mainp1( argc, argv ));
}

struct sw heapsw[SWITSZ];	/* heap for switches */

genswitch(p,n) register struct sw *p;
{
	/*	p points to an array of structures, each consisting
		of a constant value and a label.
		The first is >=0 if there is a default label;
		its value is the label number
		The entries p[1] to p[n] are the nontrivial cases
		*/
	register i;
	register CONSZ j, range;
	register dlab, swlab;

	range = p[n].sval-p[1].sval;

	if( range>0 && range <= 3*n && n>=4 ){ /* implement a direct switch */

		dlab = p->slab >= 0 ? p->slab : getlab();
#ifdef VAX
		swlab = getlab();

		/* already in r0 */
		printf("	casel	r0,$%ld,$%ld\n", p[1].sval, range);
		printf("L%d:\n", swlab);
		for( i=1,j=p[1].sval; i<=n; j++) {
			printf("	.word	L%d-L%d\n", (j == p[i].sval ? ((j=p[i++].sval), p[i-1].slab) : dlab),
				swlab);
			}

		if( p->slab >= 0 ) branch( dlab );
		else printf("L%d:\n", dlab);
#else
		if( p[1].sval ){
			printf( "	subl	#" );
			printf( CONFMT, p[1].sval );
			printf( ",d0\n" );
			}

		/* note that this is a cl; it thus checks
		   for numbers below range as well as out of range.
		   */
		printf( "	cmpl	#%ld,d0\n", range );
		printf( "	bhi	L%d\n", dlab );

		printf( "	addw	d0,d0\n" );
		printf( "	movw	pc@(6,d0:w),d0\n" );
		printf( "	jmp	pc@(2,d0:w)\n" );

		/* output table */

  /*	printf( "L%d = \n", swlab=getlab() ); */
		printf( "L%d:  \n", swlab=getlab() );

		for( i=1,j=p[1].sval; i<=n; ++j ){

			printf( "	.word	L%d-L%d\n", ( j == p[i].sval ) ?
				p[i++].slab : dlab, swlab );
			}

		if( p->slab< 0 ) deflab( dlab );
#endif
		return;

		}

	if( n>8 ) {	/* heap switch */

		heapsw[0].slab = dlab = p->slab >= 0 ? p->slab : getlab();
		makeheap(p, n, 1);	/* build heap */

		walkheap(1, n);	/* produce code */

		if( p->slab >= 0 )
			branch( dlab );
		else
			printf("L%d:\n", dlab);
		return;
	}

	/* debugging code */

	/* out for the moment
	if( n >= 4 ) werror( "inefficient switch: %d, %d", n, (int) (range/n) );
	*/

	/* simple switch code */

	for( i=1; i<=n; ++i ){
		/* already in r0 */

#ifdef VAX
		printf( "	cmpl	r0,$" );
#else
		printf( "	cmpl	#" );
#endif
		printf( CONFMT, p[i].sval );
#ifdef VAX
		printf( "\n	jeql	L%d\n", p[i].slab );
#else
		printf( ",d0\n	beq	L%d\n", p[i].slab );
#endif
		}

	if( p->slab>=0 ) branch( p->slab );
}

makeheap(p, m, n)
register struct sw *p;
{
	register int q;

	q = select(m);
	heapsw[n] = p[q];
	if( q>1 ) makeheap(p, q-1, 2*n);
	if( q<m ) makeheap(p+q, m-q, 2*n+1);
}

select(m) 
{
	register int l,i,k;

	for(i=1; ; i*=2)
		if( (i-1) > m ) break;
	l = ((k = i/2 - 1) + 1)/2;
	return( l + (m-k < l ? m-k : l));
}

walkheap(start, limit)
{
	int label;


	if( start > limit ) return;
#ifdef VAX
		printf( "	cmpl	r0,$" );
#else
		printf( "	cmpl	#" );
#endif
		printf( CONFMT, heapsw[start].sval );
#ifdef VAX
		printf( "\n	jeql	L%d\n", heapsw[start].slab );
#else
		printf( ",d0\n	beq	L%d\n", heapsw[start].slab );
#endif
	if( (2*start) > limit ) {
#ifdef VAX
		printf("	jbr 	L%d\n", heapsw[0].slab);
#else
		printf("	bra 	L%d\n", heapsw[0].slab);
#endif
		return;
	}
	if( (2*start+1) <= limit ) {
		label = getlab();
#ifdef VAX
		printf("	jgtr	L%d\n", label);
#else
		printf("	bgt	L%d\n", label);
#endif 
	} else
#ifdef VAX 
		printf("	jgtr	L%d\n", heapsw[0].slab);
#else 
		printf("	bgt	L%d\n", heapsw[0].slab);
#endif 
	walkheap( 2*start, limit);
	if( (2*start+1) <= limit ) {
		printf("L%d:\n", label);
		walkheap( 2*start+1, limit);
	}
}
