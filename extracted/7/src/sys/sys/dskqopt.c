/* dskqopt.c - dskqopt */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dskqopt  --  optimize requests to read/write/seek to the same block
 *------------------------------------------------------------------------
 */
dskqopt(p, q, drptr)
struct	dreq	*p, *q, *drptr;
{
	char	*to, *from;
	int	i;
	DBADDR	block;

	/* By definition, sync requests cannot be optimized.  Also,	*/
	/* cannot optimize read requests if already reading.		*/

	if (drptr->drop==DSYNC || (drptr->drop==DREAD && p->drop==DREAD))
		return(SYSERR);

	if (drptr->drop == DSEEK) {	/* ignore extraneous seeks	*/
		freebuf(drptr);
		return(OK);
	}

	if (p->drop == DSEEK) {		/* replace existing seeks	*/
		drptr->drnext = p->drnext;
		q->drnext = drptr;
		freebuf(p);
		return(OK);
	}

	if (p->drop==DWRITE && drptr->drop==DWRITE) {	/* dup write	*/
		drptr->drnext = p->drnext;
		q->drnext = drptr;
		freebuf(p->drbuff);
		freebuf(p);
		return(OK);
	}

	if (drptr->drop==DREAD && p->drop==DWRITE) {	/* satisfy read	*/
		to = drptr->drbuff;
		from = p->drbuff;
		for (i=0 ; i<DBUFSIZ ; i++)
			*to++ = *from++;
		return(OK);
	}

	if (drptr->drop==DWRITE && p->drop==DREAD) {	/* sat. old read*/
		block = drptr->drdba;
		from = drptr->drbuff;
		for (; p!=DRNULL && p->drdba==block ; p=p->drnext) {
			q->drnext = p->drnext;
			to = p->drbuff;
			for (i=0 ; i<DBUFSIZ ; i++)
				*to++ = *from++;
			p->drstat = OK;
			ready(p->drpid, RESCHNO);			
		}
		drptr->drnext = p;
		q->drnext = drptr;
		resched();
		return(OK);
	}
	return(SYSERR);
}
