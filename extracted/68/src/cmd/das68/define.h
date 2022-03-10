#include <stdio.h>

#define	BADOP	"\t???"
#define	IMDF	"#0x%x"

extern	long	dot;
extern	FILE	*infile;
extern	char	objcode[100];
extern	char	text[100];

struct opdesc
{			
	unsigned short mask, match;
	int (*opfun)();
	char *farg;
}; 
