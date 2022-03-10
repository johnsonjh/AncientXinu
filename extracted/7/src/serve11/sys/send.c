/* send.c - send */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  send  --  send a message to another process
 *------------------------------------------------------------------------
 */
SYSCALL	send(pid, msg)
int	pid;
int	msg;
{
	struct	pentry	*pptr;		/* receiver's proc. table addr.	*/
	char	ps;

	disable(ps);
	if (isbadpid(pid) || ( (pptr= &proctab[pid])->pstate == PRFREE)
	   || pptr->phasmsg) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pmsg = msg;		/* deposit message		*/
	pptr->phasmsg = TRUE;
	if (pptr->pstate == PRRECV)	/* if receiver waits, start it	*/
		ready(pid, RESCHYES);
	else if (pptr->pstate == PRTRECV) {
		unsleep(pid);
		ready(pid, RESCHYES);
	}
	restore(ps);
	return(OK);
}
