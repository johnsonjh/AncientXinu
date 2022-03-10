/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
SYSCALL	setnok(nok, pid)
int	nok;
int	pid;
{
	char	ps;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	return(OK);
}
