/* ttyoin.c - ttyoin */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <necuart.h>

/*------------------------------------------------------------------------
 *  ttyoin  --  output character to uart and set busy bit
 *------------------------------------------------------------------------
 */
ttyoin(ttyp, cptr)
	register struct	tty	*ttyp;
	register struct	csr	*cptr;
{
	int	count;

#ifdef	DEBUG
	kprintf("ttyoin(%x, %x)\n", ttyp, cptr);
#endif
	pokeuart(ttyp->ioaddr, 0, NECeoi);	/* End of Interrupt	*/
	if (ttyp->ehead	!= ttyp->etail)	{
		sluputch(cptr, ttyp->ebuff[ttyp->etail++]);
		if (ttyp->etail	>= EBUFLEN)
			ttyp->etail = 0;
		ttyp->tbusy = TRUE;
		return;
	}
	if (ttyp->oheld) {			/* honor flow control	*/
		ttyp->tbusy = FALSE;
		return;
	}
	if ((count=scount(ttyp->osem)) < OBUFLEN) {
		sluputch(cptr, ttyp->obuff[ttyp->otail++]);
		if (ttyp->otail	>= OBUFLEN)
			ttyp->otail = 0;
		if (count > OBMINSP)
			signal(ttyp->osem);
		else if	( ++(ttyp->odsend) == OBMINSP) {
			ttyp->odsend = 0;
			signaln(ttyp->osem, OBMINSP);
		}
		ttyp->tbusy = TRUE;
	} else {
		ttyp->tbusy = FALSE;
	}
}
