/* dl.s -- phase0, phase1 */

#define	PH1LEN	r0
#define	PH1ADR	r1
#define	DELAY	r2
#define	ENTRY	r3
#define	IOBYTE	r4
#define	STATUS	r5
#define	SIDREG	r6

#include <procreg.h>
#include <vdownload.h>

/*----------------------------------------------------------------------------
 * Phase 0 of downloader.  Loaded at location P1BSTRT using console deposit
 * commands, loading stops with the 0xffffffff value.  Reads phase1 of
 * the downloader from the high speed line into a high location in memory
 * (address in r0) and then executes phase1.  Assumes processor in kernel
 * mode with interrupts disabled, memory mapping disabled, on interrupt
 * stack.
 *
 *   Arguments:
 *	r0 - length in bytes of phase1
 *	r1 - address of highest byte at which phase1 is to be loaded + 1
 *	r2 - time delay before performing autostart
 *	r3 - entry point for performing autostart (if r2 >= 0)
 *----------------------------------------------------------------------------
 */
	.text
phase0:					/* no entry mask -- not called	*/
	movab	tmpstk, sp		/* give stack some memory	*/
	mfpr	$SID, SIDREG		/* get system ID, if 8600, 	*/
	bicl2	$SYSTYPE, SIDREG	/* setup RXCS and TXCS		*/
	cmpl	SIDREG, $VAX8600SID
	jneq	loadphase1		/* if not 8600, go get input	*/
	mtpr	$RXCS8600, $RXCS	/* enable 8600 console I/O	*/
	mtpr	$TXCS8600, $TXCS
loadphase1:
	bsbb	getbyte			/* read phase1 in reverse order	*/
	cvtlb	IOBYTE, IOBYTE		/* (8600) chop off high bytes	*/
	cmpb	IOBYTE, $ESC		/* if escaped, get real byte	*/
	jneq	storebyte		/* otherwise go on		*/
	bsbb	getbyte
	bicl2	$0xffffff40, IOBYTE	/* remove stuffed bit		*/
storebyte:
	movb	IOBYTE, -(PH1ADR)	/* store the byte		*/
	sobgtr	PH1LEN, loadphase1	/* go back to input more	*/
	movl	PH1ADR, sp		/* give stack some memory	*/
	jmp	(PH1ADR)		/* run the loaded phase1	*/

getbyte:
	mfpr	$RXCS, STATUS		/* get input status		*/
	jbc	$SLURDYBIT,STATUS,getbyte/* go back if input not ready	*/
	mfpr	$RXDB, IOBYTE		/* get byte and store it	*/
	rsb

	.align	2			/* align loc ctr to longword	*/
	.long	P0MARKER		/* marks end of phase0		*/
tmpstk:					/* stack for phase 0		*/

/* definitions for phase1 */
#undef	PH1LEN
#define	SUM	r0			/* the checksum			*/
/*		r1	is PH1ADR
		r2	is DELAY
		r3	is ENTRY	
		r4	is IOBYTE
		r5	is STATUS
		r6	is SIDREG					*/
#define	LENGTH	r7			/* length of packet in words	*/
#define	BASEADR	r8			/* destination addr of packet	*/
#define	COUNT	r9			/* loop control variable	*/
#define	PACKADR	r10			/* current address in packet	*/
#define	INWORD	r11			/* the current input word	*/
#define	SAVCLKV	r12			/* saved copy of clock vector	*/

/*-----------------------------------------------------------------------
 * Phase1 -- load program into low memory over console line.  Phase1 is
 *		loaded into high memory by phase0
 *-----------------------------------------------------------------------
 */
phase1:					/* no entry mask -- not called	*/
	clrl	INWORD			/* initialize high word to 0	*/
getsoh:
	bsbw	rawgetc			/* scan for SOH			*/
	cmpb	IOBYTE, $SOH
	jneq	getsoh
	clrl	SUM			/* start check summing		*/
	bsbw	getwsum			/* get low word of base address	*/
	movw	INWORD, BASEADR
	bsbw	getwsum			/* get high word of base address*/
	insv	INWORD,$16,$16,BASEADR
	bsbw	getwsum			/* get packet length		*/
	movl	INWORD, LENGTH
	cmpw	LENGTH, $PACKSIZE	/* test packet length		*/
	jgtr	sendnak			/* invalid length--retry	*/

	movl	LENGTH,	COUNT		/* set up to get packet		*/
	jeql	getsum
	movaw	packbuf, PACKADR
getwbuf:
	bsbw	getwsum			/* get word for packet		*/
	movw	INWORD, (PACKADR)+	/* store in buffer		*/
	sobgtr	COUNT, getwbuf		/* get rest of packet		*/

getsum:
	bsbw	getwsum			/* test checksum		*/
	tstw	SUM			/* sum should now be zero	*/
	jeql	copypack		/* yes==> copy packet to memory	*/

sendnak:				/* invalid packet - send NAK &	*/
	movb	$NAK, IOBYTE		/* retry			*/
	bsbw	rawputc
	jbr	getsoh

copypack:				/* valid packet--place it in	*/
	movl	LENGTH, COUNT		/* memory and send ACK		*/
	jeql	sendack			/* zero length packet		*/
	movaw	packbuf, PACKADR
storeword:
	movw	(PACKADR)+, (BASEADR)+
	sobgtr	COUNT, storeword

sendack:
	movb	$ACK, IOBYTE
	bsbw	rawputc
	tstw	LENGTH			/* if zero length, that's all	*/
	jneq	getsoh			/* else get another packet	*/

	tstl	DELAY			/* see if autostart desired	*/
	jeql	startit			/* no delay, just start it	*/
					/* if delay<0, no autostart-->	*/
					/* wait for user to break to	*/
					/* console mode			*/
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
	jmp	(ENTRY)			/* should never return		*/

haltit:
					/* if it's a UVAXII can halt,	*/
					/* otherwise must do tight loop	*/
					/* (halting may cause "reboot")	*/
	mfpr	$SID, SIDREG		/* get system id register value	*/
	bicl2	$SYSTYPE, SIDREG	/* mask off system id		*/
	cmpl	SIDREG, $UVAXIISID	/* if UVAXII, issue halt	*/
	jneq	occupycpu		/*   otherwise do tight loop	*/
	movw	$QMHALT,*$QMCPMBX	/* raise console prog mailbox	*/
	halt

getwsum:				/* getwsum--get word, chksuming	*/
	bsbw	getc			/* get low order byte		*/
	movb	IOBYTE, INWORD
	bsbw	getc			/* get high order byte		*/
	insv	IOBYTE,$8,$8,INWORD	/* high byte into word		*/
	addw2	INWORD, SUM		/* maintain checksum		*/
	rsb

getc:					/* get byte, honoring ESC	*/
	bsbw	rawgetc
	cmpb	$ESC, IOBYTE		/* is it the ESC?		*/
	jneq	1f			/* no ==> return the byte	*/
	bsbw	rawgetc			/* yes==> get the real byte	*/
	bicl2	$0xffffff40, IOBYTE	/* remove stuffed bit		*/
1:
	rsb

rawgetc:
	mfpr	$RXCS, STATUS		/* get status			*/
	jbc	$SLURDYBIT,STATUS,rawgetc/* go back if input not ready	*/
	mfpr	$RXDB, IOBYTE		/* get byte			*/
	cvtlb	IOBYTE, IOBYTE		/* (for 8600)			*/
	rsb

rawputc:				/* rawputc -- put character	*/
	mfpr	$TXCS, STATUS		/* get status			*/
	jbc	$SLURDYBIT,STATUS,rawputc/* if busy, go back		*/
	mtpr	IOBYTE, $TXDB		/* send byte			*/
	rsb

packbuf:				/* phase0 leaves PACKSIZE*2	*/
					/* bytes here for packet buffer	*/
