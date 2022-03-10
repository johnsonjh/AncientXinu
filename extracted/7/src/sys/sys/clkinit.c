/* clkinit.c - clkinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>

/* real-time clock variables and sleeping process queue pointers	*/

#ifdef	RTCLOCK
int	count6;			/* counts in 60ths of a second 6-0	*/
int	count10;		/* counts in 10ths of a second 10-0	*/
int	clmutex;		/* mutual exclusion for time-of-day	*/
long	clktime;		/* current time in seconds since 1/1/70	*/
int     defclk;			/* non-zero, then deferring clock count */
int     clkdiff;		/* deferred clock ticks			*/
int     slnempty;		/* FALSE if the sleep queue is empty	*/
int     *sltop;			/* address of key part of top entry in	*/
				/* the sleep queue if slnempty==TRUE	*/
int     clockq;			/* head of queue of sleeping processes  */
int	preempt;		/* preemption counter.	Current process */
				/* is preempted when it reaches zero;	*/
				/* set in resched; counts in ticks	*/
int	clkruns;		/* set TRUE iff clock exists by setclkr	*/
#else
int	clkruns = FALSE;	/* no clock configured; be sure sleep	*/
#endif				/*   doesn't wait forever		*/

/*
 *------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
clkinit()
{
	int *vector;

	vector = (int *) CVECTOR;	/* set up interrupt vector	*/
	*vector++ = clkint;
	*vector = DISABLE;
	setclkr();
	preempt = QUANTUM;		/* initial time quantum		*/
	count6 = 6;			/* 60ths of a sec. counter	*/
	count10 = 10;			/* 10ths of a sec. counter	*/
	clmutex = screate(1);		/* semaphore for tod clock	*/
	clktime = 0L;			/* initially a low number	*/
	slnempty = FALSE;		/* initially, no process asleep	*/
	clkdiff = 0;			/* zero deferred ticks		*/
	defclk = 0;			/* clock is not deferred	*/
	clockq = newqueue();		/* allocate clock queue in q	*/
}
