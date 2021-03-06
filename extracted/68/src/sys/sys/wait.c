/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL	wait(sem)
	int	sem;
{
	struct	sentry	*sptr;
	struct	pentry	*pptr;

#ifdef	DEBUG
	dotrace("wait", &sem, 1);
#endif
	disable();
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore();
		return(SYSERR);
	}
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		resched();
	}
	restore();
	return(OK);
}
