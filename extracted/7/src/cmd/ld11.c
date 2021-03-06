/*
 *  link editor - Source modified to be just a PDP 11 cross-loader 
 *                for the VAX using the portable archive format               
 *                October 1981          Steve Salisbury
 */

/*
 *  This file produces:
 *	ld11	for the vax
 *
 *  Modified to take advantage of library indices (see xlib.c).
 *
 *  Eric Cooper
 *  July, 1980
 */

#include <stdio.h>
#include <signal.h>
#include <ar.h>
#include <sys/types.h>
#include <sys/stat.h>


/* various machine dependent definitions */
#include "/usr/Xinu/include/pdp11/pagsiz.h"
#include "/usr/Xinu/include/pdp11/a.out.h"
#define LIBNAME  "/usr/Xinu/lib/lib"


/* high and low words of longs are reversed between vax and pdp11 */
typedef struct	{
	short hiword;
	short loword;
} longword;

fixl(p)
register longword *p;
{
	register short t;

	t=p->hiword; p->hiword=p->loword; p->loword=t;
}

long atoi();

typedef	char *STRING;
typedef	short BOOL;
#define TRUE	1
#define FALSE	0

#define	COMM	05	/* internal use only */

/*
 * for rounding to word boundaries
 */
#define	HW	0
#define	FW	01
#define	DW	03

#define TMASK	(~0)
#define	RELFLG	01

#define NOVLY	16

#define	NROUT	500
#define	NSYM	1103
#define	NSYMPR	1000

char	premeof[] = "Premature EOF";

typedef struct {
	long	loc;
} LIBLIST;

/* overlay management */
int	vindex;
typedef struct {
	int	argsav;
	int	symsav;
	LIBLIST	*libsav;
	STRING	vname;
	long	ctsav, cdsav, cbsav;
	long	offt, offd, offb, offtr, offdr, offs;
} OVERLAY;
OVERLAY	vnodes[NOVLY];

/* input management */
typedef struct {
	short	*fakeptr;
	int	bno;
	int	nibuf;
	int	nuser;
	char	buff[BSIZE];
} PAGE;

PAGE	page[2];

struct {
	short	*fakeptr;
	int	bno;
	int	nibuf;
	int	nuser;
} fpage;

typedef struct {
	char	*ptr;
	int	bno;
	int	nibuf;
	long	size;
	long	pos;
	PAGE	*pno;
} STREAM;

STREAM text;
STREAM reloc;

struct	ar_hdr archdr;
long           archdr_size;
long           archdr_date;

struct	exec filhdr;

/*
 * one entry for each archive member referenced;
 * set in first pass; needs restoring for overlays
 */

LIBLIST	liblist[NROUT];
LIBLIST	*libp = liblist;

/* for fast loading of libraries */
#define ARXFILNAME      "__.SYMDEF     "  

typedef struct {
	char x_name[8];
	long x_loc;	/* byte addr of module within library */
} XDEF;
XDEF	*xdefs;		/* sorted table of defined symbols in library */
int	nxdefs;

/* symbol management */
typedef struct nlist SYMBOL;

typedef struct {
	int locindex;		/* index to symbol in file */
	SYMBOL *locsymbol;	/* ptr to symbol table */
} LOCAL;

SYMBOL	cursym;			/* current symbol */
SYMBOL	*symtab;		/* actual symbols */
SYMBOL	*lastsym;		/* last symbol entered */
SYMBOL	*nextsym;		/* next available symbol table entry */
int	nsym;
SYMBOL	*hshtab[NSYM+2];	/* hash table for symbols */
LOCAL	*local;
short	indxtab[NSYM];
#define symhash(p)	(indxtab[(p)-symtab])

/* internal symbols */
SYMBOL	*p_etext;
SYMBOL	*p_edata;
SYMBOL	*p_end;
SYMBOL	*entrypt;

/* flags */
int	xflag;		/* discard local symbols */
int	Xflag;		/* discard locals starting with 'L' */
int	Sflag;		/* discard all except locals and globals */
int	rflag;		/* preserve relocation bits, don't define common */
int	arflag;		/* original copy of rflag */
int	sflag;		/* discard all symbols */
int	dflag;		/* define common even with rflag */
int	iflag;		/* I/D space separated */
int	nflag = 0;	/* pure procedure */
BOOL	vflag;		/* overlays used */
int	Oflag;		/* set magic # to 405 (overlay) */

int	ofilfnd;
char	*ofilename	= "l.out";
int	infil;
char	*filname;

long	textbase;
/* cumulative sizes set in pass 1 */
long	tsize;
long	dsize;
long	bsize;
long	trsize;
long	drsize;
long	ssize;

/* symbol relocation; both passes */
long	ctrel;
long	cdrel;
long	cbrel;
long	ctorel;
long	cdorel;
long	cborel;

int	errlev;
int	delarg	= 4;


FILE	*tout;
FILE	*dout;
char	*doutn	= "";
FILE	*trout;
char	*troutn	= "";
FILE	*drout;
char	*droutn	= "";
FILE	*sout;
char	*soutn	= "";

char	*mktemp();
char 	get();
char	getb();
long	get3();
long	getl();
SYMBOL	**lookup();
FILE	*tcreat();
long	round();
SYMBOL	**slookup();
SYMBOL	*lookloc();
long	step();
XDEF	*bsearch();

delexit()
{
	unlink("l.out");
	if (*doutn)
		unlink(doutn);
	if (*troutn)
		unlink(troutn);
	if (*droutn)
		unlink(droutn);
	if (*soutn)
		unlink(soutn);
	if (delarg==0)
		chmod(ofilename, 0777 &~ umask(0));
	exit(delarg);
}

main(argc, argv)
char **argv;
{
	register int c, i; 
	int num;
	register char *ap, **p;
	BOOL found; 
	int vscan;
	char save;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, delexit);
	if (argc == 1)
		exit(4);
	p = argv+1;

	nextsym = symtab = (SYMBOL *)calloc(NSYM, sizeof(SYMBOL));
	if (symtab == NULL)
		error(1, "Memory overflow");

	/* scan files once to find symdefs */
	for (c=1; c<argc; c++) {
		filname = 0;
		ap = *p++;

		if (*ap == '-') {
			for (i=1; ap[i]; i++) {
			switch (ap[i]) {
			case 'o':
				if (++c >= argc)
					error(1, "Bad output file");
				ofilename = *p++;
				ofilfnd++;
				continue;

			case 'u':
			case 'e':
				if (++c >= argc)
					error(1, "Bad 'use' or 'entry'");
				enter(slookup(*p++));
				if (ap[i]=='e')
					entrypt = lastsym;
				continue;

			case 'D':
				if (++c >= argc)
					error(1, "-D: arg missing");
				num = atoi(*p++);
				if (dsize>num)
					error(1, "-D: too small");
				dsize = num;
				continue;

			case 'T':
				if (++c >= argc)
					error(1, "-T: arg missing");
				if (tsize!=0)
					error(1, "-T: too late, some text already loaded");
				textbase = atoi(*p++);
				continue;

			case 'l':
				save = ap[--i]; 
				ap[i]='-';
				load1arg(&ap[i]); 
				ap[i]=save;
				break;

			case 'x':
				xflag++;
				continue;

			case 'X':
				Xflag++;
				continue;

			case 'S':
				Sflag++; 
				continue;

			case 'r':
				rflag++;
				arflag++;
				continue;

			case 's':
				sflag++;
				xflag++;
				continue;

			case 'n':
				nflag++;
				continue;

			case 'N':
				nflag = 0;
				continue;

			case 'd':
				dflag++;
				continue;

			case 'i':
				iflag++;
				continue;

			case 'v':
				if (++c >= argc)
					error(1, "-v: arg missing");
				vflag = TRUE;
				vscan = vindex; 
				found = FALSE;
				while (--vscan>=0)
					if (strcmp(vnodes[vscan].vname, *p) == 0) {
						found = TRUE;
						break;
					}
				if (found) {
					endload(c, argv);
					restore(vscan);
				} else
					record(c, *p);
				p++;
				continue;

			case 'O':
				Oflag++;
				continue;
			default:
				error(1, "bad flag");
			} /*endsw*/
			break;
			} /*endfor*/
		} else
			load1arg(ap);
	}
	endload(argc, argv);
	exit(0);
}

/* used after pass 1 */
long	torigin;
long	dorigin;
long	borigin;
long	database;

endload(argc, argv)
int argc; 
char **argv;
{
	register int c, i; 
	long dnum;
	register char *ap, **p;

/*
	realloc(symtab, nextsym-symtab);
*/
	filname = 0;
	middle();
	setupout();
	local = (LOCAL *)malloc(NSYMPR*sizeof(*local));
	if (local == NULL)
		error(1,"Memory overflow");
	p = argv+1;
	libp = liblist;
	for (c=1; c<argc; c++) {
		ap = *p++;
		if (*ap == '-') {
			for (i=1; ap[i]; i++) {
			switch (ap[i]) {
			case 'D':
				for (dnum = atoi(*p); dorigin<dnum; dorigin++) {
					putc(0, dout);
					if (rflag)
						putc(0, drout);
				}
			case 'T':
			case 'u':
			case 'e':
			case 'o':
			case 'v':
				++c; 
				++p;

			default:
				continue;

			case 'l':
				ap[--i]='-'; 
				load2arg(&ap[i]);
				break;
			} /*endsw*/
			break;
			} /*endfor*/
		} else
			load2arg(ap);
	}
	finishout();
}

record(c, nam)
int c; 
STRING nam;
{
	register OVERLAY *v;

	v = &vnodes[vindex++];
	v->argsav = c;
	v->symsav = nextsym-symtab;
	v->libsav = libp;
	v->vname = nam;
	v->offt = tsize; 
	v->offd = dsize; 
	v->offb = bsize; 
	v->offtr = trsize;
	v->offdr = drsize;
	v->offs = ssize;
	v->ctsav = ctrel; 
	v->cdsav = cdrel; 
	v->cbsav = cbrel;
}

restore(vscan)
int vscan;
{
	register OVERLAY *v;
	register SYMBOL *saved,*sp;

	v = &vnodes[vscan];
	vindex = vscan+1;
	libp = v->libsav;
	ctrel = v->ctsav; 
	cdrel = v->cdsav; 
	cbrel = v->cbsav;
	tsize = v->offt; 
	dsize = v->offd; 
	bsize = v->offb; 
	trsize = v->offtr;
	drsize = v->offdr;
	ssize = v->offs;
	saved = symtab + v->symsav;
	sp = nextsym;
	while (sp>saved)
		hshtab[symhash(--sp)]=0;
	nextsym = saved;
}

/* scan file to find defined symbols */
load1arg(cp)
register char *cp;
{
	long loc;
	int olderrlev;

	switch (getfile(cp)) {

	case 0:			/* regular object file */
		load1(0, 0L);
		break;

	case 1:			/* regular archive */
                loc = SARMAG;     
	regular:
		while (loc = step(loc))
			;
		libp++->loc = -1;
		break;

	case 2:			/* archive with index */
		ldxar(symtab);
		libp++->loc = -1;
		break;

	case 3:			/* out of date index */
		olderrlev = errlev;
		error(0, "index out of date (warning)");
		errlev = olderrlev;
                loc = sizeof(int) + sizeof(archdr) + round(archdr_size, 1);
		goto regular;
	}
	close(infil);
}

long
step(loc)
long loc;
{
	dseek(&text, loc, (long)sizeof(archdr));
	if (text.size <= 0)
		return (0);
	mget(&archdr, sizeof(archdr), &text);
        archdr_size = atoi(archdr.ar_size);
        archdr_date = atoi(archdr.ar_date);
#ifdef DEBUG
	printf("%.14s, %d bytes, %s", 
		archdr.ar_name,
                archdr_size,
                ctime(&archdr_date));
#endif
	if (load1(1, loc+sizeof(archdr))) {
		libp++->loc = loc;
		if(libp >= liblist + NROUT)
			error(1,"liblist overflow");
	}
        return (loc + round(archdr_size, 1) + sizeof(archdr));
}

/*
 * Load an indexed archive.
 * Try to resolve all undefined externals above symptr.
 */

ldxar(symptr)
SYMBOL *symptr;
{
	register SYMBOL *sp;
	register XDEF *xp;
	SYMBOL *before;

	for (sp = symptr; sp < nextsym; sp++)
		if (sp->n_type == N_EXT+N_UNDF) {
			xp = bsearch(sp->n_name);
			if (xp == (XDEF *)0)
				continue;
			before = nextsym;
			step(xp->x_loc);
			if (nextsym != before)
				ldxar(before);
		}
}

XDEF *
bsearch(name)
STRING name;
{
	register bottom = 1;
	register top = nxdefs;
	register mid;
	register XDEF *p;
	short n;

#ifdef DEBUG
	printf("%.8s ", name);
#endif
	while (top >= bottom) {
		mid = (bottom + top) / 2;
		p = &xdefs[mid-1];
		n = strcmpn(name, p->x_name, 8);
		if (n > 0)
			bottom = mid + 1;
		else if (n < 0)
			top = mid - 1;
		else
			return (p);
	}
#ifdef DEBUG
	printf("not found\n");
#endif
	return ((XDEF *) 0);
}

/* single file or archive member */
load1(libflg, loc)
long loc;
{
	register SYMBOL *sp;
	SYMBOL *savnext;
	int ndef, nlocal, type, mtype;

	readhdr(loc);
	ctrel = tsize;
	cdrel += dsize;
	cbrel += bsize;
	ndef = 0;
	nlocal = sizeof(cursym);
	savnext = nextsym;
	if ((filhdr.a_flag&RELFLG) == 1) {
		error(0, "No relocation bits");
		return(0);
	}
	loc += sizeof(filhdr) + 2*(filhdr.a_text + filhdr.a_data);
	dseek(&text, loc, (long)filhdr.a_syms);
	while (text.size > 0) {
		symget(&cursym, &text);
		type = cursym.n_type;
		if (Sflag) {
			mtype = type&N_TYPE;
			if (mtype == 1 || mtype > 4)
				continue;
		}
		if ((type&N_EXT)==0) {
			if (Xflag==0 || cursym.n_name[0]!='L' || type&STABTYPES)
				nlocal += sizeof cursym;
			continue;
		}
		symreloc();
		if (enter(lookup()))
			continue;
		if ((sp = lastsym)->n_type != N_EXT+N_UNDF)
			continue;
		if (cursym.n_type == N_EXT+N_UNDF) {
			if (cursym.n_value > sp->n_value)
				sp->n_value = cursym.n_value;
			continue;
		}
		if (sp->n_value != 0 && cursym.n_type == N_EXT+N_TEXT)
			continue;
		ndef++;
		sp->n_type = cursym.n_type;
		sp->n_value = cursym.n_value;
	}
	if (libflg==0 || ndef) {
		tsize += filhdr.a_text;
		dsize += round(filhdr.a_data, FW);
		bsize += round(filhdr.a_bss, FW);
		ssize += nlocal;
		return(1);
	}
	/*
	 * No symbols defined by this library member.
	 * Rip out the hash table entries and reset the symbol table.
	 */
	while (nextsym>savnext)
		hshtab[symhash(--nextsym)]=0;
	return(0);
}

middle()
{
	register SYMBOL *sp, *symp;
	long csize, t, corigin, ocsize;
	int nund, rnd;
	char s;

	torigin = 0; 
	dorigin = 0; 
	borigin = 0;

	p_etext = *slookup("_etext");
	p_edata = *slookup("_edata");
	p_end = *slookup("_end");
	/*
	 * If there are any undefined symbols, save the relocation bits.
	 */
	symp = nextsym;
	if (rflag==0) {
		for (sp = symtab; sp<symp; sp++)
			if (sp->n_type==N_EXT+N_UNDF && sp->n_value==0
				&& sp!=p_end && sp!=p_edata && sp!=p_etext
			) {
				rflag++;
				dflag = 0;
				break;
			}
	}
	if (rflag) 
		sflag = iflag = 0;
	if (rflag) 
		nflag = Oflag = 0;
	/*
	 * Assign common locations.
	 */
	csize = 0;
	if (dflag || rflag==0) {
		ldrsym(p_etext, tsize, N_EXT+N_TEXT);
		ldrsym(p_edata, dsize, N_EXT+N_DATA);
		ldrsym(p_end, bsize, N_EXT+N_BSS);
		for (sp = symtab; sp<symp; sp++) {
			if ((s=sp->n_type)==N_EXT+N_UNDF && (t = sp->n_value)!=0) {
				rnd = HW;
				csize = round(csize, rnd);
				sp->n_value = csize;
				sp->n_type = N_EXT+COMM;
				ocsize = csize;	
				csize += t;
			}
			if (((s&TMASK) == N_EXT+N_UNDF) && (s & STABTYPES)) {
				sp->n_value = ocsize;
				sp->n_type = (s & STABTYPES) | (N_EXT+COMM);
			}
		}
	}
	/*
	 * Now set symbols to their final value
	 */
	csize = round(csize, FW);
	torigin = textbase;
	t = (nflag || iflag) ? round(tsize, 077) : tsize;
	database = (iflag ? 0 : (nflag ? round(t, PAGRND) : t));
	dorigin = database;
	corigin = dorigin + dsize;
	borigin = corigin + csize;
/*
*/
	cdorel = 0;
	cborel = dsize+csize;
	nund = 0;
	for (sp = symtab; sp<symp; sp++) switch (sp->n_type & TMASK) {
	case N_EXT+N_UNDF:
		if (!arflag)
			errlev |= 01;
		if ((arflag==0 || dflag) && sp->n_value==0) {
			if (nund==0)
				printf("Undefined:\n");
			nund++;
			printf("%.8s\n", sp->n_name);
		}
		continue;

	case N_EXT+N_ABS:
	default:
		continue;

	case N_EXT+N_TEXT:
		sp->n_value += torigin;
		continue;

	case N_EXT+N_DATA:
		sp->n_value += dorigin;
		continue;

	case N_EXT+N_BSS:
		sp->n_value += borigin;
		continue;

	case N_EXT+COMM:
		sp->n_type = (sp->n_type & STABTYPES) | (N_EXT+N_BSS);
		sp->n_value += corigin;
		continue;
	}
	if (!arflag && nund)	/* don't bother with second pass */
		delexit();
	if (sflag || xflag)
		ssize = 0;
	bsize += csize;
	nsym = ssize / sizeof(SYMBOL);
}

ldrsym(asp, val, type)
long val;
SYMBOL *asp;
{
	register SYMBOL *sp;

	if ((sp = asp) == 0)
		return;
	if (sp->n_type != N_EXT+N_UNDF || sp->n_value) {
		printf("%.8s: ", sp->n_name);
		error(0, "Multiply defined (internal)");
		return;
	}
	sp->n_type = type;
	sp->n_value = val;
}

extern char _sibuf[BUFSIZ]; /* the space is forced upon us; might as well use it */

setupout()
{
	int bss;

	tout = fopen(ofilename, "w");
	if (tout==NULL)
		error(1, "cannot create output");
	setbuf(tout,_sibuf);
	dout = tcreat(&doutn, "/tmp/ldaaXXXXX");
	if (sflag==0 || xflag==0)
		sout = tcreat(&soutn, "/tmp/ldbaXXXXX");
	if (rflag) {
		trout = tcreat(&troutn, "/tmp/ldcaXXXXX");
		drout = tcreat(&droutn, "/tmp/lddaXXXXX");
	}
	filhdr.a_magic = (Oflag ? A_MAGIC4 :
				(iflag ? A_MAGIC3 :
					(nflag ? A_MAGIC2 : A_MAGIC1)));
	filhdr.a_text = (nflag || iflag ? round(tsize, 077) : tsize);
	filhdr.a_data = dsize;
	filhdr.a_bss = bsize;
	filhdr.a_unused = 0;
	filhdr.a_flag = (rflag==0);
	filhdr.a_syms = sflag? 0: (ssize + (sizeof cursym)*(nextsym-symtab));
	if (entrypt) {
		if (entrypt->n_type!=N_EXT+N_TEXT)
			error(0, "Entry point not in text");
		else
			filhdr.a_entry = entrypt->n_value | 01;
	} else
		filhdr.a_entry=0;
	fwrite(&filhdr, sizeof(filhdr), 1, tout);
}

FILE *
tcreat(namep, name)
char **namep, *name;
{
	register FILE *fp;
	register char *tnm;

	tnm = mktemp(name);
	if ((fp = fopen(tnm, "w")) == NULL)
		error(1, "Cannot create temp file");
	chmod(tnm, 0600);
	*namep = tnm;
	return(fp);
}

load2arg(acp)
char *acp;
{
	register char *cp;
	register LIBLIST *lp;

	cp = acp;
	if (getfile(cp) == 0) {
		while (*cp)
			cp++;
		while (cp >= acp && *--cp != '/');
		mkfsym(++cp);
		load2(0L);
	} else {	/* scan archive members referenced */
		for (lp = libp; lp->loc != -1; lp++) {
			dseek(&text, lp->loc, (long)sizeof(archdr));
			mget(&archdr, sizeof(archdr), &text);
			mkfsym(archdr.ar_name);
			load2(lp->loc + (long)sizeof(archdr));
		}
		libp = ++lp;
	}
	close(infil);
}

load2(loc)
long loc;
{
	register SYMBOL *sp;
	register LOCAL *lp;
	register int symno;
	int type, mtype;

	readhdr(loc);
	ctrel = torigin;
	cdrel += dorigin;
	cbrel += borigin;
	/*
	 * Reread the symbol table, recording the numbering
	 * of symbols for fixing external references.
	 */
	lp = local;
	symno = -1;
	loc += sizeof(filhdr);
	dseek(&text, loc+2*(filhdr.a_text+filhdr.a_data), (long)filhdr.a_syms);
	while (text.size > 0) {
		symno++;
		symget(&cursym, &text);
		symreloc();
		type = cursym.n_type;
		if (Sflag) {
			mtype = type&N_TYPE;
			if (mtype == 1 || mtype > 4)
				continue;
		}
		if ((type&N_EXT) == 0) {
			if (!sflag&&!xflag&&
				(!Xflag||cursym.n_name[0]!='L'||type&STABTYPES))
				symwrite(&cursym, 1, sout);
			continue;
		}
		if ((sp = *lookup()) == 0)
			error(1, "internal error: symbol not found");
		if (cursym.n_type == N_EXT+N_UNDF) {
			if (lp >= local+NSYMPR)
				error(1, "Local symbol overflow");
			lp->locindex = symno;
			lp++->locsymbol = sp;
			continue;
		}
		if (cursym.n_type & STABTYPES) {
			continue;
		}
		if (cursym.n_type!=sp->n_type || cursym.n_value!=sp->n_value) {
			printf("%.8s: ", cursym.n_name);
			error(0, "Multiply defined");
		}
	}
	dseek(&text, loc, (long)filhdr.a_text);
	dseek(&reloc, loc+filhdr.a_text+filhdr.a_data, (long)filhdr.a_text);
	load2td(lp, ctrel, tout, trout);
	dseek(&text, loc+filhdr.a_text, (long)filhdr.a_data);
	dseek(&reloc, loc+2*filhdr.a_text+filhdr.a_data, (long)filhdr.a_data);
	load2td(lp, cdrel, dout, drout);
	while (filhdr.a_data&FW) {
		putc(0, dout); filhdr.a_data++;
	}
	torigin += filhdr.a_text;
	dorigin += filhdr.a_data;
	borigin += filhdr.a_bss;
	cdorel += filhdr.a_data;
	cborel += filhdr.a_bss;
}

load2td(lp, creloc, b1, b2)
LOCAL *lp;
long creloc;
FILE *b1, *b2;
{
	register short r;
	register short t;
	register SYMBOL *sp;
	register char *cp;
	short rr, tt;

	for (;;) {
		/*
		 * The pickup code is copied from "get" for speed.
		 */

		/* next text or data word */
		cp = (char *)&tt;
		/* first byte */
		if (--text.size <= 0) {
			if (text.size < 0)
				break;
			text.size++;
			*cp++ = get(&text);
		} else if (--text.nibuf < 0) {
			text.nibuf++;
			text.size++;
			*cp++ = get(&text);
		} else
			*cp++ = *text.ptr++;
		/* second byte */
		if (--text.size <= 0) {
			if (text.size < 0)
				break;
			text.size++;
			*cp++ = get(&text);
		} else if (--text.nibuf < 0) {
			text.nibuf++;
			text.size++;
			*cp++ = get(&text);
		} else
			*cp++ = *text.ptr++;
		t = tt;

		/* next relocation word */
		cp = (char *)&rr;
		/* first byte */
		if (--reloc.size <= 0) {
			if (reloc.size < 0)
				error(1, "Relocation error");
			reloc.size++;
			*cp++ = get(&reloc);
		} else if (--reloc.nibuf < 0) {
			reloc.nibuf++;
			reloc.size++;
			*cp++ = get(&reloc);
		} else
			*cp++ = *reloc.ptr++;
		/* second byte */
		if (--reloc.size <= 0) {
			if (reloc.size < 0)
				error(1, "Relocation error");
			reloc.size++;
			*cp++ = get(&reloc);
		} else if (--reloc.nibuf < 0) {
			reloc.nibuf++;
			reloc.size++;
			*cp++ = get(&reloc);
		} else
			*cp++ = *reloc.ptr++;
		r = rr;

		switch (r&016) {

		case R_TEXT:
			t += ctrel;
			break;

		case R_DATA:
			t += cdrel;
			break;

		case R_BSS:
			t += cbrel;
			break;

		case R_UNDF:
			sp = lookloc(lp, (int)r);
			if (sp->n_type==N_EXT+N_UNDF) {
				r = (r&01) + ((nsym+(sp-symtab))<<4) + R_UNDF;
				break;
			}
			t += sp->n_value;
			r = (r&01) + ((sp->n_type-(N_EXT+N_ABS))<<1);
			break;
		}
		if (r&01)
			t -= creloc;
		puts(t, b1);
		if (rflag)
			puts(r, b2);
	}
}

finishout()
{
	if (nflag || iflag)
		while (tsize&077) {
			putc(0, tout); tsize++;
			if (rflag)
				putc(0, trout);
		}
	fclose(dout);
	copy(doutn);
	if (rflag) {
		fclose(trout);
		copy(troutn);
		fclose(drout);
		copy(droutn);
	}
	if (sflag==0) {
		if (xflag==0) {
			fclose(sout);
			copy(soutn);
		}
		symwrite(symtab, nextsym-symtab, tout);
	}
	fclose(tout);
	if (!ofilfnd) {
		unlink("a.out");
		link("l.out", "a.out");
		ofilename = "a.out";
	}
	delarg = errlev;
	delexit();
}

copy(np)
char *np;
{
	register c;
	register FILE *fp;

	if ((fp = fopen(np, "r")) == NULL)
		error(1, "cannot recopy output");
	while ((c = getc(fp)) != EOF)
		putc(c, tout);
	fclose(fp);
}

mkfsym(s)
char *s;
{

	if (sflag || xflag)
		return;
	strcpyn(cursym.n_name, s, 8);
	cursym.n_type = N_TYPE;
	cursym.n_value = torigin;
	symwrite(&cursym, 1, sout);
}

mget(loc, n, sp)
register STREAM *sp;
register char *loc;
{
	register char *p;

	if ((sp->nibuf -= n) >= 0) {
		if ((sp->size -= n) > 0) {
			p = sp->ptr;
			sp->pos += n;
			do
				*loc++ = *p++;
			while (--n);
			sp->ptr = p;
			return;
		} else
			sp->size += n;
	}
	sp->nibuf += n;
	do {
		*loc++ = get(sp);
	} while (--n);
}

symwrite(sp, n, f)
SYMBOL *sp;
FILE *f;
{
	fwrite(sp, sizeof(*symtab), n, f);
}

symget(sp,f) 
SYMBOL *sp; 
STREAM *f; 
{
	mget(sp,sizeof(*sp),f);
}

dseek(sp, loc, s)
register STREAM *sp;
long loc, s;
{
	register PAGE *p;
	register b, o;
	int n;

	b = loc>>BSHIFT;
	o = loc&BMASK;
	if (o&01)
		error(1, "loader error; odd offset");
	--sp->pno->nuser;
	if ((p = &page[0])->bno!=b && (p = &page[1])->bno!=b)
		if (p->nuser==0 || (p = &page[0])->nuser==0) {
			if (page[0].nuser==0 && page[1].nuser==0)
				if (page[0].bno < page[1].bno)
					p = &page[0];
			p->bno = b;
			lseek(infil, loc & ~(long)BMASK, 0);
			if ((n = read(infil, p->buff, sizeof(p->buff))) < 0)
				n = 0;
			p->nibuf = n;
	} else
		error(1, "No pages");
	++p->nuser;
	sp->bno = b;
	sp->pno = p;
	if (s != -1) {sp->size = s; sp->pos = 0;}
	sp->ptr = p->buff + o;
	if ((sp->nibuf = p->nibuf-o) <= 0)
		sp->size = 0;
}

char
get(asp)
STREAM *asp;
{
	register STREAM *sp;

	sp = asp;
	if ((sp->nibuf -= sizeof(char)) < 0) {
		dseek(sp, ((long)(sp->bno+1)<<BSHIFT), (long)-1);
		sp->nibuf -= sizeof(char);
	}
	if ((sp->size -= sizeof(char)) <= 0) {
		if (sp->size < 0)
			error(1, premeof);
		++fpage.nuser;
		--sp->pno->nuser;
		sp->pno = (PAGE *)&fpage;
	}
	sp->pos += sizeof(char);
	return (*sp->ptr++);
}

getfile(acp)
register STRING acp;
{
        char arcmag[SARMAG];
	struct stat st;
	static char buf[256];

	infil = -1;
	archdr.ar_name[0] = '\0';
	if (acp[0] == '-' && acp[1] == 'l') {
		if(acp[2] == '\0')
			acp = "-la";
                sprintf(buf, "%s%s.a", LIBNAME, &acp[2]);
		filname = buf;
/****************************************************************/
/* This trick checks for "/lib/lib..." before "/v1/lib/lib..." */
/* but we aren't using those libraries so we don't do this.     */ 
/****************************************************************/
/*              if ((infil = open(filname+4, 0)) != -1)         */
/*                      filname += 4;                           */
/****************************************************************/
	}
	else
		filname = acp;
	if (infil == -1 && (infil = open(filname, 0)) < 0)
		error(1, "cannot open");
#ifdef DEBUG
	printf("getfile(%s)\n", filname);
#endif
	page[0].bno = page[1].bno = -1;
	page[0].nuser = page[1].nuser = 0;
	text.pno = reloc.pno = (PAGE *)&fpage;
	fpage.nuser = 2;
        dseek(&text, 0L, (long)SARMAG);
	if (text.size <= 0)
		error(1, premeof);
        mget(arcmag, SARMAG, &text);
        if (strcmpn(arcmag,ARMAG,SARMAG))
		return (0);
        dseek(&text, (long)SARMAG, (long)sizeof(archdr));
	if (text.size <= 0)
		error(1, premeof);
	mget(&archdr, sizeof(archdr), &text);
        archdr_size = atoi(archdr.ar_size);
        archdr_date = atoi(archdr.ar_date);
        if (strcmpn(archdr.ar_name, ARXFILNAME, 14) != 0)
		return (1);
	fstat(infil, &st);
        if (st.st_mtime > archdr_date)
		return (3);
        xdefs = (XDEF *)malloc(archdr_size);
	if (xdefs == NULL)
		error(1, "Memory overflow");
        dseek(&text, (long)SARMAG+sizeof(archdr), archdr_size);
	if (text.size <= 0)
		error(1, premeof);
        mget(xdefs, archdr_size, &text);
        nxdefs = archdr_size / sizeof(XDEF);
	return (2);
}

SYMBOL **lookup()
{
	register int i; 
	register SYMBOL **hp;
	register char *cp;

	i = 0;
	
	for (cp = cursym.n_name; cp < &cursym.n_name[8];)
		i = (i<<1) + *cp++;
	hp = &hshtab[(i&077777)%NSYM+2];
	while (*hp != 0)
		if (strcmpn((*hp)->n_name, cursym.n_name, 8) != 0) {
			if (++hp >= &hshtab[NSYM+2])
				hp = hshtab;
		}
		else
			break;
	return(hp);
}

SYMBOL **slookup(s)
char *s;
{
	strcpyn(cursym.n_name, s, 8);
	cursym.n_type = N_EXT+N_UNDF;
	cursym.n_value = 0;
	return(lookup());
}

enter(hp)
register SYMBOL **hp;
{
	register SYMBOL *sp;

	if (*hp==0) {
		if ((nextsym-symtab)>=NSYM)
			error(1, "Symbol table overflow");
		*hp = lastsym = sp = nextsym++;
		strcpyn(sp->n_name, cursym.n_name, 8);
		sp->n_type = cursym.n_type;
		symhash(sp) = hp-hshtab;
		sp->n_value = cursym.n_value;
		return(1);
	} else {
		lastsym = *hp;
		return(0);
	}
}

symreloc()
{
	switch (cursym.n_type) {

	case N_TEXT:
	case N_EXT+N_TEXT:
		cursym.n_value += ctrel;
		return;

	case N_DATA:
	case N_EXT+N_DATA:
		cursym.n_value += cdrel;
		return;

	case N_BSS:
	case N_EXT+N_BSS:
		cursym.n_value += cbrel;
		return;

	case N_EXT+N_UNDF:
		return;
	}
	if (cursym.n_type&N_EXT)
		cursym.n_type = N_EXT+N_ABS;
}

error(n, s)
char *s;
{
	if (errlev==0)
		printf("ld:");
	if (filname) {
		printf("%s", filname);
                if (archdr.ar_name[0])
			printf("(%.14s)", archdr.ar_name);
		printf(": ");
	}
	printf("%s\n", s);
	if (n)
		delexit();
	errlev = 2;
}

SYMBOL *
lookloc(lp, r)
register LOCAL *lp;
{
	register LOCAL *clp;
	register sn;

	sn = (r>>4) & 07777;
	for (clp = local; clp<lp; clp++)
		if (clp->locindex == sn)
			return(clp->locsymbol);
	error(1, "Local symbol botch");
}

readhdr(loc)
long loc;
{
	long *p; int i;

	dseek(&text, loc, (long)sizeof(filhdr));
	mget(&filhdr, sizeof(filhdr), &text);
	if (filhdr.a_magic!=A_MAGIC1 && filhdr.a_magic!=A_MAGIC2 &&
		filhdr.a_magic!=A_MAGIC3 && filhdr.a_magic!=A_MAGIC4)
			error(1,"Bad magic number");
	if (filhdr.a_text&01 || filhdr.a_data&01) {
		printf("tsize=%X  dsize=%X\n",filhdr.a_text,filhdr.a_data);
		error(1, "Text/data size odd");
	}
	if (filhdr.a_magic == A_MAGIC2) {
		cdrel = -round(filhdr.a_text, PAGRND);
		cbrel = cdrel - filhdr.a_data;
	} else if (filhdr.a_magic == A_MAGIC1) {
		cdrel = -filhdr.a_text;
		cbrel = cdrel - filhdr.a_data;
	} else
		error(1, "Bad format");
}

long
round(v, r)
long v;
unsigned r;
{
	v += r;
	v &= ~(long)r;
	return(v);
}

writel(p, n, f)
long *p;
FILE *f;
{
	while (n--)
		putl(*p++, f);
}

putl(w, f)
FILE *f;
long w; 
{
	fixl(&w);
	fwrite(&w,sizeof(long),1,f);
}

long
getl(sp) 
STREAM *sp; 
{
	long t; 
	
	mget(&t,4,sp);
	fixl(&t);
	return(t);
}

puts(w, f)
FILE *f; 
short w; 
{
	fwrite(&w,sizeof(short),1,f);
}

char
*gets(sp)
STREAM *sp; 
{
	short t; mget(&t,2,sp); return((char *)t);
}

#ifdef DEBUG
prsymtab()
{
	register SYMBOL *sp;

	for (sp = symtab; sp < nextsym; sp++)
		printf("%.8s\n", sp->n_name);
}
#endif
