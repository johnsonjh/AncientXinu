/* phelp.c - phelp */

#include <conf.h>
#include <kernel.h>
#include "pshell.h"
#include "pshellc.h"

/*------------------------------------------------------------------------
 *  phelp  -  'help' command for pseudo shell
 *------------------------------------------------------------------------
 */
phelp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, inc;

	if ( (inc=(Psh.pncmds+3)/4) <= 0)
		inc = 1;
	fprintf(stdout, "Commands are:\n");
	for(i=0 ; i<inc && i<Psh.pncmds ; i++) {
		fprintf(stdout, "  ");
		for (j=i ; j<Psh.pncmds ; j+=inc)
			fprintf(stdout, "%-16s", pcmds[j].cmdnam);
		fprintf(stdout, "\n");
	}
	return(OK);
}
