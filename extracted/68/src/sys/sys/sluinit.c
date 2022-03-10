/* sluinit.c - sluinit */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#ifdef	lsi11
#	include <slu.h>
#else
#	include <necuart.h>
#endif
	

/*------------------------------------------------------------------------
 *  sluinit  --  low-level slu initialization 
 *------------------------------------------------------------------------
 */
int	sluinit(devptr, ttyp)
	struct	devsw	*devptr;
	struct	tty	*ttyp;
{
	register struct	csr	*cptr;
	int	i;

#ifdef	DEBUG
	dotrace("sluinit", &cptr, 2);
#endif

	cptr = realcsr(devptr);			/* CSR of current chan.	*/

#ifdef	lsi11
	i = slugetch(cptr);			/* Clear receiver	*/
	slutdisable(cptr, ttyp);		/* Disable trans. ints	*/
	slurenable(cptr, ttyp);			/* Enable receiver ints	*/
#else
	pokeuart(cptr, WR0, NECchanres);	/* Channel reset	*/
	for(i=0; i<10; i++);			/* Delay for reset	*/
	pokeuart(ttyp->ioaddr, WR2A, 0);	/* Non-vectored inters.	*/
	pokeuart(cptr, WR4, NECrx1sb|NEC9600b);
						/* 1 stop bit, 9600 baud*/
	pokeuart(cptr, WR3, NECrx8bt|NECrxena);	/* 8 bits/char, Rx ena	*/
	pokeuart(cptr, WR5, NECtxdtr|NECtx8bt|NECtxena|NECtxrts);
						/* 8 bits/char, Tx ena, */
						/* Data Terminal Ready,	*/
						/* Ready To Send	*/
	pokeuart(cptr, WR0, NECtxires);		/* Tx Interrupt Reset	*/
	sluputch(cptr, '\n');			/* Set Buffer Empty Flag*/
	slutenable(cptr, ttyp);			/* Enable trans. ints	*/
	slurenable(cptr, ttyp);			/* Enable receiver ints	*/

#endif
}
