#
/* PDP-11 Assembler for the VAX Pass 1 */

#include "vars2.h"

struct afile txtp, relp;

char *a_outp = "a.out";
char *a_tmp1, *a_tmp2, *a_tmp3;

long dataseeks[3] = { 020L, 0L, 0L};	/* Seeks for txt, data, bss */
long relseeks[3];		/* Relocation info seeks for txt, data, bss */
long symseek;
long *tseekp = &dataseeks[0];
long *rseekp = &relseeks[0];
struct header txtmagic = {
	0407, 0, 0, 0, 0, 0, 0, 0,
	};

char brtab[BRLEN/8];
int brtabp;
short brdelt;
struct symbol *fbbufp;
int defund;
short savdot[3];
short datbase;
short bssbase;
struct symbol *xsymbol;
int ch;
int errflg;
int line;
int savop;
struct symbol *curfb[10];
struct symbol *nxtfb[10];
short numval;
char passno;
struct symbol *usymtab, *endtable;
char argb[22];
int adrbuf[6];
int fout;
char etype;
short eval;
char *endcore;
short *memptr;
