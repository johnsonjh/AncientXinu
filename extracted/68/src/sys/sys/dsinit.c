/* dsinit.c - dsinit */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

#ifdef	Ndsk
struct	dsblk	dstab[Ndsk];
#endif
int	dskdbp, dskrbp;

/*------------------------------------------------------------------------
 *  dsinit  --  initialize disk drive device
 *------------------------------------------------------------------------
 */
dsinit(devptr)
	struct	devsw	*devptr;
{
	struct	xbdcb	*xptr;
	struct	dsblk	*dsptr;
	struct	dtc	*dtptr;
	char	status;

#ifdef	DEBUG
	dotrace("dsinit", &devptr, 1);
#endif
	disable();
	devptr->dvioblk = dsptr = &dstab[ devptr->dvminor ];
	dsptr->dcsr    = devptr->dvcsr;
	dsptr->dreqlst = DRNULL;
	dsptr->dnum    = devptr->dvnum;
	dsptr->dibsem  = screate(1);
	dsptr->dflsem  = screate(1);
	dsptr->ddirsem = screate(1);
	dsptr->dnfiles = 0;
	dsptr->ddir    = (char *) getbuf(dskdbp);
	iosetvec(devptr->dvnum, dsptr, dsptr);
	dtptr = dsptr->dcsr;

	status = dtptr->dt_ccsr;	/* Clear Command Completion SR */

	/* read directory block: setup read command then start interface */

	xptr = & dsptr->ddcb;
	xptr->xop    = (char) XOREAD;
	xptr->xunit  = (char) 0;
	xptr->xcntl  = (char) XRETRY;
	xptr->xladdr = lbyte(DIRBLK);
	xptr->xmaddr = mbyte(DIRBLK);
	xptr->xcount = (char) 1;
	dscopy(xptr, MULTI(BUS_COM), sizeof(struct xbdcb));

	dtptr->dt_darl = lbyte(BUS_BUF);
	dtptr->dt_darm = mbyte(BUS_BUF);
	dtptr->dt_darh = hbyte(BUS_BUF);
	dtptr->dt_carl = lbyte(BUS_COM);
	dtptr->dt_carm = mbyte(BUS_COM);
	dtptr->dt_carh = hbyte(BUS_COM);
	dtptr->dt_csr   = DTGO;
	while ( (dtptr->dt_csr & DTDONE) == 0)
		;
	if ( dtptr->dt_ccsr & DTERROR)
		panic("Disk error");
	restore();
	return(OK);
}
