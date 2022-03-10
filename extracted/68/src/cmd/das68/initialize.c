#include <stdio.h>
#include <a.out.h>
#include "define.h"

extern	struct	exec	a;

GetHeader()
{
	fread(&a, sizeof(struct exec), 1, infile);

	if (N_BADMAG(a)) {
		fprintf(stderr,"file is not in a.out format\n");
		exit(1);
	}
	printf("%d Bytes in the text segment\n",a.a_text);
}
			
int	Relsize;
struct	relocation_info	*Relocation;

GetRelocationInfo()
{
	Relsize = (a.a_trsize + a.a_drsize) / sizeof (struct relocation_info);
	Relocation = (struct relocation_info *) malloc(a.a_trsize + a.a_drsize);
	fseek(infile, sizeof (struct exec) + a.a_text + a.a_data, 0);
	fread(Relocation, sizeof (struct relocation_info), Relsize, infile);
}

int	Symsize;
struct	nlist	*Syms, *Symsort;

GetSymbolTable()
{
	Symsize = a.a_syms/sizeof(struct nlist);
	Syms = (struct nlist *)malloc(a.a_syms);
	Symsort = (struct nlist *)malloc(a.a_syms);
	fseek(infile, N_SYMOFF(a),0);
	fread(Syms, 1, a.a_syms, infile);
}

static	char	*Strings;
static	int	Strsize;

GetStrings()
{
	struct 	nlist 	*s, *t;
	int 	nlistcomp();

	Strings = (char *)malloc(Strsize);
	fseek(infile, N_STROFF(a), 0);
	fread(&Strsize, sizeof(int), 1, infile);
	fread(Strings, 1, Strsize, infile);

	/* Adjust the string offsets in the symbol table to 
	   actual char pointers into the strings table */

	for (s=Syms, t=Symsort; s < &Syms[Symsize]; s++, t++) {
		s->n_un.n_name = (char *)(s->n_un.n_strx + (int)Strings -
		    (sizeof(Strsize)));
		*t = *s;
	}
	qsort(Symsort, Symsize, sizeof (struct nlist), nlistcomp);
}
