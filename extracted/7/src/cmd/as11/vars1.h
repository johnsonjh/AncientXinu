#include <stdio.h>

struct symbol {
	char s_name[8];
	struct sinfo {
		char s_type;
		char s_index;
		short s_value;
	} s_info;
};

#define DOT	symtab[0].s_info.s_value
#define DOTREL	symtab[0].s_info.s_type
#define DOTDOT	symtab[1].s_info.s_value

#define ASEOF	04

#define CTRL	0
#define TERM	1
#define BLNK	2
#define OPER	3
#define QUOT	4
#define SQOT	5
#define COMM	6
#define IDEN	7
#define STRN	8
#define ESCP	9
#define ORSY	10

#define HSHSIZE	3001

char *sbrk();
FILE *fcreat();

extern char iobuf[BUFSIZ], _sobuf[BUFSIZ];

extern char a_tmp1[], a_tmp2[], a_tmp3[], globfl[];
extern char *argv2[];
extern FILE *pof, *fbfil;
extern char errflg;
extern struct symbol *usymtab;
extern char *symend;

extern char **curarg;
extern int ifflg, line;

extern char ch;
extern char fileflg;
extern char numsymbol;

extern short savdot[];
extern struct sinfo nxtfb;

extern int eval;
extern char etype;
extern short curfb[];
extern char curfbr[];

extern int saveop;
extern short numval;
extern char chartab[];

extern struct symbol *hshtab[];
extern struct symbol symbol;

extern struct symbol symtab[];
extern int nargs;
extern char *endcore;

extern char unglob;
