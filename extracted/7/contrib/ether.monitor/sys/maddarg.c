/* maddarg.c - maddarg */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "../h/mshell.h"

/*------------------------------------------------------------------------
 *  maddarg  -  copy shell arguments to area reserved on process stack
 *------------------------------------------------------------------------
 */
maddarg(pid, nargs, len)
int	pid;				/* process to receive arguments	*/
int	nargs;				/* number of arguments to copy	*/
int	len;				/* size of arg. area (in bytes)	*/
{
	struct	pentry	*pptr;
	int	*fromarg;
	int	*toarg;
	char	*to;

	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate != PRSUSP)
		return(SYSERR);
	toarg = (int *) ( ((unsigned)pptr->pbase) - (unsigned)len );
	to = (char *) (toarg + (nargs + 2));
	*toarg++ = toarg + 1;
	for (fromarg= &Msh.mtokens ; nargs > 0 ; nargs--) {
		*toarg++ = to;
		strcpy(to, *fromarg++);
		to += strlen(to) + 1;		
	}
	*toarg = 0;
	return;
}
