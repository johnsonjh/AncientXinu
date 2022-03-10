/* dsseek.c - dsseek */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dsseek  --  schedule a request to move the disk arm
 *------------------------------------------------------------------------
 */
dsseek(devptr, block)
	struct	devsw	*devptr;
	DBADDR	block;
{
	struct	dreq	*drptr;

#ifdef	DEBUG1
	dotrace("dsseek", &devptr, 2);
#endif
	disable();
	if ( (drptr = (struct dreq *)getbuf(dskrbp)) == SYSERR)
		return(SYSERR);
	drptr->drdba = block;
	drptr->drpid = currpid;
	drptr->drbuff = NULL;
	drptr->drop = DSEEK;

	/* enqueued with normal policy like other read/write requests */

	dskenq(drptr, devptr->dvioblk);
	restore();
	return(OK);
}
