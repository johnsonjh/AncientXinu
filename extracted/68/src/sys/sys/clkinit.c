/* clkinit.c - clkinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#ifdef	RTCLOCK

/*
 *------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
clkinit()
{
	int junk;
	WORD *vector;

#ifdef	DEBUG
	dotrace("clkinit", NULL, 0);
#endif
	vector = (WORD *) CVECTOR;	/* set up interrupt vector	*/
	*vector = (WORD) clkint;
#ifdef	DEBUG
	dotrace("setclkr", NULL, 0);
#endif
	setclkr();
	preempt = QUANTUM;		/* initial time quantum		*/
	count6 = 6;			/* 60ths of a sec. counter	*/
}
#endif
