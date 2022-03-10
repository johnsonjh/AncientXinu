#
/* PDP-11 Assembler for the VAX Pass 0 */

#include "vars1.h"

char a_tmp1[] =	"/tmp/atm1a";
char a_tmp2[] = "/tmp/atm2a";
char a_tmp3[] = "/tmp/atm3a";

char iobuf[BUFSIZ];

char *symend;

short savdot[3];
struct symbol *hshtab[HSHSIZE];
FILE *pof, *fbfil;
char fileflg, errflg, ch;

struct symbol symbol;
int line;
int ifflg;
int nargs;
char **curarg;
int saveop;
short numval;
struct symbol *usymtab;
char *endcore;
