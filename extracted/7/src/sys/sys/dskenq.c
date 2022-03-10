/* dskenq.c - dskenq */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dskenq  --  enqueue a disk request and start I/O if disk not busy
 *------------------------------------------------------------------------
 */
dskenq(drptr, dsptr)
	struct	dreq	*drptr;
	struct	dsblk	*dsptr;
{
	struct	dreq	*p, *q;		/* q follows p through requests	*/
	DBADDR	block;
	int	st;

	if ( (q=dsptr->dreqlst) == DRNULL ) {
		dsptr->dreqlst = drptr;
		drptr->drnext = DRNULL;
		dskstrt(dsptr);
		return(DONQ);
	}
	block = drptr->drdba;
	for (p = q->drnext ; p != DRNULL ; q=p,p=p->drnext) {
		if (p->drdba==block && (st=dskqopt(p, q, drptr)!=SYSERR))
				return(st);
		if ( (q->drdba <= block && block < p->drdba) ||
		     (q->drdba >= block && block > p->drdba)   ) {
			drptr->drnext = p;
			q->drnext = drptr;
			return(DONQ);
		}
	}
	drptr->drnext = DRNULL;
	q->drnext = drptr;
	return(DONQ);
}
