/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
SYSCALL	suspend(pid)
	int	pid;			/* id of process to suspend	*/
{
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	char	ps;			/* saved processor status	*/
	int	prio;			/* priority returned		*/

	disable(ps);
	if (isbadpid(pid) || pid==NULLPROC ||
	 ((pptr= &proctab[pid])->pstate!=PRCURR && pptr->pstate!=PRREADY)) {
		restore(ps);
		return(SYSERR);
	}
	if (pptr->pstate == PRREADY) {
		dequeue(pid);
		pptr->pstate = PRSUSP;
	} else {
		pptr->pstate = PRSUSP;
		resched();
	}
	prio = pptr->pprio;
	restore(ps);
	return(prio);
}
