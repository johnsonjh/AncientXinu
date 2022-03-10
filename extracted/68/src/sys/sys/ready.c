/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int	ready (pid, resch)
	int	pid;			/* id of process to make ready	*/
	int	resch;			/* reschedule afterward?	*/
{
	register struct	pentry	*pptr;

#ifdef	DEBUG
	dotrace("ready", &pid, 2);
#endif
	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	insert(pid,rdyhead,pptr->pprio);
	if (resch)
		resched();
	return(OK);
}
