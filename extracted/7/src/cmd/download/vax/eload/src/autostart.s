/* autostart.s -- autostart */

#define	ASM
#include <vaxdl.h>
#include <procreg.h>
#include <qbus.h>

#define	COUNT	r1			/* loop control variable	*/
#define	SAVCLKV	r2			/* saved copy of clock vector	*/
#define	DELAY	r3			/* # of seconds to count down	*/
#define	SIDVAL	r4			/* stores content of SID reg	*/

/*
 * autostart -- implement ethernet downloader autostart options
 * 		arguments: (starting with topmost on stack)
 *		   delay time in seconds (-1 == no autostart)
 *		   entry point
 *
*/
	.text
	.globl	_autostart
_autostart:
	.word	0x0			/* entry mask: save no regs	*/
	movl	4(ap), DELAY		/* get delay in seconds		*/
	tstl	DELAY			/* see if autostart desired	*/
	jeql	startit			/* no delay, just start it	*/
					/* if delay<0, no autostart-->	*/
	jlss	haltit

/* implement autostart-- UVAXI and UVAXII don't have ICCS INT bit to	*/
/* busy-wait on, so drop interrupt priority to count down		*/

	movl	*$CVECTOR, SAVCLKV	/* save old clock vector	*/
	movab	tickhandler+1, *$CVECTOR/* point to autostart handler	*/
	movl	$100, COUNT		/* counts ticks (1/100th sec)	*/
	mtpr	$-CLKINTVL, $NICR	/* set up clock with timer val	*/
	mtpr	$STRTCLK, $ICCS		/* (UVAXEN are hardwired)	*/
	mtpr	$CLKIPL, $IPL		/* lower interrupt priority	*/

/* loop until DELAY == 0 if autostarting, or until user breaks into	*/
/* console mode, if no autostart and not UVAXII				*/

occupycpu:
	jbr	occupycpu
					/* tickhandler catches clock	*/
	.align	2			/* interrupts on interrupt stk	*/
tickhandler:
	mtpr	$CLEARINT, $ICCS	/* clear clock interrupt	*/
	decl	COUNT			/* count 1/100th sec ticks	*/
	jneq	1f			/* not a full second yet, go on	*/
	decl	DELAY			/* decrement count of seconds	*/
	jeql	arestore		/* count completed, start code	*/
	movl	$100, COUNT		/* count another second		*/
1:	rei

arestore:				/* restore from autostart	*/
	mtpr	$DISABLE, $IPL		/* disable interrupts		*/
	movl	SAVCLKV, *$CVECTOR	/* restore old clock vector	*/
	mtpr	$0, $NICR		/* restore clock		*/
	mtpr	$0, $ICCS

startit:				/* go to entry point of code	*/
	calls	$0, *8(ap)		/* should never return		*/

haltit:
					/* if it's a UVAXII can halt,	*/
					/* otherwise must do tight loop	*/
					/* (halting may cause "reboot")	*/
	mfpr	$SID, SIDVAL		/* get system id register value	*/
	bicl2	$SYSTYPE, SIDVAL	/* mask off system id		*/
	cmpl	SIDVAL, $UVAXIISID	/* if UVAXII, issue halt	*/
	jneq	occupycpu		/*   otherwise do tight loop	*/
	movw	$QMHALT, *$QMCPMBX	/* raise console prog mailbox	*/
	halt				/* UVAXII actually halts	*/
