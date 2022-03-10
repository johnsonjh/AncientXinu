/* dsinter.c - dsinter */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dsinter  --  process disk interrupt (DTC interface; XEBEC controller)
 *------------------------------------------------------------------------
 */
INTPROC	dsinter(dsptr)
	struct	dsblk	*dsptr;
{
	struct	dtc	*dtptr;
	struct	dreq	*drptr;

	dtptr = dsptr->dcsr;
	drptr = dsptr->dreqlst;
	if (drptr == DRNULL) {
		panic("Disk interrupt when disk not busy");
		return;
	}
	if (dtptr->dt_csr & DTERROR)
		drptr->drstat = SYSERR;
	else
		drptr->drstat = OK;
	if ( (dsptr->dreqlst=drptr->drnext) != DRNULL)
		dskstrt(dsptr);
	switch (drptr->drop) {

		case DREAD:
		case DSYNC:
			ready(drptr->drpid, RESCHYES);
			return;

		case DWRITE:
			freebuf(drptr->drbuff);
			/* fall through */
		case DSEEK:
			freebuf(drptr);
	}
}
