#include <stdio.h>
#include <sys/types.h>
#include "x.h"

/*
 * pm - external variables, structure definitions
 */

extern	short		*text;
extern	short		*core;
extern	FILE		*txtfd;
extern	FILE		*corefd;
extern	char		*txtfile;
extern	char		*corefile;
extern	struct nlist	*symtab, *esymtab;
extern	struct nlist	*ssymtab, *essymtab;
extern	struct nlist	nl[];
extern	struct core11	c_header;
extern	struct exec	a_out;
extern	int		errflg;
extern	int		allopts, popt, sopt, topt;
extern	int		verbose;

typedef	short		INT;
typedef	long		L_INT;
typedef short		boolean;

struct reglist{
	char	*rname;
	INT	roffs;
};

#define	EVEN	~01
#define MAXOFF	1023	/* maximum reasonable displacement from symbol */
#define	MAXARGS	6	/* maximum reasonable number of arguments */
#define	RECLIMIT 20	/* maximum reasonable recursion depth */

/* errflg values */
#define	NOCFN	1
