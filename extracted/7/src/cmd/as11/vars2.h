struct symbol {
	char s_type;
	char s_index;
	short s_value;
};

struct afile {
	short *f_ptr; 
	short *f_end;
	long f_seek;
	short f_buffer[256];
};

struct	header {	/* a.out header */
	short     	h_magic;	/* magic number */
	short		h_text; 	/* size of text segment */
	short		h_data; 	/* size of initialized data */
	short		h_bss;  	/* size of unitialized data */
	short		h_syms; 	/* size of symbol table */
	short		h_entry; 	/* entry point */
	short		h_unused;	/* not used */
	short		h_flag; 	/* relocation info stripped */
};

#define	TXTSIZ txtmagic.h_text
#define DATSIZ txtmagic.h_data
#define BSSSIZ txtmagic.h_bss
#define SYMSIZ txtmagic.h_syms
#define STKSIZ txtmagic.h_entry
#define EXORIG txtmagic.h_flag

#define DOT	symtab[0].s_value
#define DOTREL	symtab[0].s_type
#define	DOTDOT	symtab[1].s_value

#define BRLEN 1024
#define ASEOF	04
#define BR	0400
#define JMP	0100

extern struct header txtmagic;
extern struct afile txtp, relp;
extern char *a_tmp1, *a_tmp2, *a_tmp3, *a_outp, *a_outp1, *a_outp2;
extern char *endcore, passno;
extern short *memptr;
extern struct symbol *fbbufp, *endtable;
extern int outmod, brtabp, defund, fout, errflg;
extern short bssbase, datbase;
extern short savdot[];
extern long relseeks[], dataseeks[], symseek;
extern struct symbol *usymtab, *curfb[], *nxtfb[];

extern long *tseekp, *rseekp;

extern struct symbol symtab[], *xsymbol;
extern short brdelt, numval, eval;
extern char etype;
extern int savop, line;
extern int adrbuf[];

extern char argb[];
extern char brtab[];
