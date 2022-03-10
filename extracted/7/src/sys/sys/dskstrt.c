/* dskstrt.c - dskstrt */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dskstrt  --  start an I/O operation on a disk device
 *------------------------------------------------------------------------
 */
dskstrt(dsptr)
	struct	dsblk	*dsptr;
{
	struct	xbdcb	*xptr;
	struct	dtc	*dtptr;
	struct	dreq	*drptr;

	/* build command for controller */

	drptr = dsptr->dreqlst;
	xptr = & dsptr->ddcb;
	xptr->xop    = (char) drptr->drop;		/* opcode	*/
	xptr->xunit  = (char) 0;			/* top addr bits*/
	xptr->xmaddr = (char) ((drptr->drdba>>8)&0377);	/* mid addr bits*/
	xptr->xladdr = (char) (drptr->drdba & 0377);	/* low addr bits*/
	xptr->xcount = (char) 1;			/* num of blocks*/
	xptr->xcntl  = (char) XRETRY;			/* retry code	*/

	/* feed command to controller through interface */

	dtptr = dsptr->dcsr;
	dtptr->dt_dar = drptr->drbuff;
	dtptr->dt_car = xptr;
	dtptr->dt_xdar = dtptr->dt_xcar = 0;
	dtptr->dt_csr = DTINTR | DTGO;
}
