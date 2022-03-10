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

	if ( (q=(devptr->dvioblk)->dreqlst) == DRNULL )
		return(OK);
	drptr = (struct dreq *) getbuf(dskrbp);
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
