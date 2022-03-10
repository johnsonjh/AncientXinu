/* dsksync.c - dsksync */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dsksync  --  wait for all outstanding disk requests before returning
 *------------------------------------------------------------------------
 */
dsksync(devptr)
struct	devsw	*devptr;
{
	struct	dreq	*drptr, *p, *q;
	int	stat;

#ifdef	DEBUG1
	dotrace("dsksync", &devptr, 1);
#endif
	if ( (q=((struct dsblk *)devptr->dvioblk)->dreqlst) == DRNULL )
		return(OK);
	if ( (drptr = (struct dreq *)getbuf(dskrbp)) == (struct dreq *)SYSERR)
		return(SYSERR);
	drptr->drdba = 0;
	drptr->drpid = currpid;
	drptr->drbuff = NULL;
	drptr->drop = DSYNC;
	drptr->drnext = DRNULL;

	/* place at end of request list */

	for (p=q->drnext ; p!=DRNULL ; q=p,p=p->drnext)
		;
	q->drnext = drptr;
	drptr->drstat = SYSERR;
	suspend(currpid);
	stat = drptr->drstat;
	freebuf(drptr);
	return(stat);
}
