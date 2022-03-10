/* mhelp.c - mhelp */

#include <conf.h>
#include <kernel.h>
#include "../h/mshell.h"
#include "../h/mshellc.h"

extern	Bool	found;

/*------------------------------------------------------------------------
 *  mhelp  -  'help' command for pseudo shell
 *------------------------------------------------------------------------
 */
int mhelp(stdout)
int	stdout; 
{
	int	i, j, inc;

	if ( (inc=(Msh.mncmds+3)/4) <= 0)
		inc = 1;
	fprintf(stdout, "Commands are:\n");
	for(i=0 ; i<inc && i<Msh.mncmds ; i++) {
		fprintf(stdout, "  ");
		for (j=i ; j<Msh.mncmds ; j+=inc)
			fprintf(stdout, "%-16s", mcmds[j].cmdnam);
		fprintf(stdout, "\n");
	}
	found = TRUE;
	return(OK);
}
