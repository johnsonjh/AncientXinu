/* dsread.c - dsread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dsread  --  read a block from a disk device
 *------------------------------------------------------------------------
 */
dsread(devptr, buff, block)
struct	devsw	*devptr;
char	*buff;
DBADDR	block;
{
	struct	dreq	*drptr;
	int	stat;

#ifdef	DEBUG
	dotrace("dsread", &devptr, 3);
#endif
	disable();
	if ( (drptr=(struct dreq *)getbuf(dskrbp)) == SYSERR )
		return(SYSERR);
	drptr->drdba = block;
	drptr->drpid = currpid;
	drptr->drbuff = BUS_BUF;
	drptr->drop = DREAD;
	if ( (stat=dskenq(drptr, devptr->dvioblk)) == DONQ) {
		suspend(currpid);
		stat = drptr->drstat;
	}
	dscopy(MULTI(BUS_BUF), buff, DBUFSIZ);
	freebuf(drptr);
	restore();
	return(stat);
}
