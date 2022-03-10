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
	struct	dsblk	*dsptr;
	struct	dtc	*dtptr;
	int	status;
	char	ps;

	disable(ps);
	devptr->dvioblk = dsptr = &dstab[ devptr->dvminor ];
	dsptr->dcsr    = devptr->dvcsr;
	dsptr->dreqlst = DRNULL;
	dsptr->dnum    = devptr->dvnum;
	dsptr->dibsem  = screate(1);
	dsptr->dflsem  = screate(1);
	dsptr->ddirsem = screate(1);
	dsptr->dnfiles = 0;
	dsptr->ddir    = getbuf(dskdbp);
	iosetvec(devptr->dvnum, dsptr, dsptr);

	/* read directory block: setup read command then start interface */

	dsptr->ddcb.xop    = (char) XOREAD;
	dsptr->ddcb.xunit  = (char) 0;
	dsptr->ddcb.xcntl  = (char) XRETRY;
	dsptr->ddcb.xmaddr = (char) ((DIRBLK>>8)&(0377));
	dsptr->ddcb.xladdr = (char) (DIRBLK&0377);
	dsptr->ddcb.xcount = (char) 1;
	dtptr = dsptr->dcsr;
	dtptr->dt_dar   = dsptr->ddir;
	dtptr->dt_car   = &dsptr->ddcb;
	dtptr->dt_xdar  = dtptr->dt_xcar = NULL;
	dtptr->dt_csr   = DTGO;
	while ( ( (status=dtptr->dt_csr) & DTDONE) == 0)
		;
	if (status & DTERROR)
		panic("Disk error");
	restore(ps);
	return(OK);
}
