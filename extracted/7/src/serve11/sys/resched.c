/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRCURR.
 *------------------------------------------------------------------------
 */
int	resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio))
		return(OK);

	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */

	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	STKCHK
	if ( *( (int *)nptr->pbase  ) != MAGIC ) {
		kprintf("Bad magic pid=%d, value=%o, at %o\n",
			currpid, *( (int *)nptr->pbase ), nptr->pbase);
		panic("stack corrupted");
	}
	if ( ((unsigned)nptr->pregs[SP]) < ((unsigned)nptr->plimit) ) {
		kprintf("Bad SP pid=%d (%s), lim=%o will be %o\n",
			currpid, nptr->pname, nptr->plimit,
			nptr->pregs[SP]);
		panic("stack overflow");
	}
#endif
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	ctxsw(optr->pregs,nptr->pregs);

	/* The OLD process returns here when resumed. */
	return(OK);
}
