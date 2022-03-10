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

#ifdef	DEBUG
	dotrace("dskstrt", &dsptr, 1);
#endif
	/* build command for controller */

	drptr = dsptr->dreqlst;
	xptr  = &dsptr->ddcb;
	xptr->xop    = (char) drptr->drop;		/* opcode	*/
	xptr->xunit  = (char) 0;			/* top addr bits*/
	xptr->xladdr = lbyte(drptr->drdba);		/* low addr bits*/
	xptr->xmaddr = mbyte(drptr->drdba);		/* mid addr bits*/
	xptr->xcount = (char) 1;			/* num of blocks*/
	xptr->xcntl  = (char) XRETRY;			/* retry code	*/
	dscopy(xptr, MULTI(BUS_COM), sizeof(struct xbdcb));

	/* feed command to controller through interface */

	if (xptr->xop == DWRITE) 
		/* since writes are asynchronous, must do copy last */
		dscopy(drptr->drbuff, MULTI(BUS_BUF), DBUFSIZ);

	dtptr = dsptr->dcsr;

	dtptr->dt_darl = lbyte(BUS_BUF);
	dtptr->dt_darm = mbyte(BUS_BUF);
	dtptr->dt_darh = hbyte(BUS_BUF);

	dtptr->dt_carl = lbyte(BUS_COM);
	dtptr->dt_carm = mbyte(BUS_COM);
	dtptr->dt_carh = hbyte(BUS_COM);

	dtptr->dt_csr  = DTINTR | DTGO;
}
