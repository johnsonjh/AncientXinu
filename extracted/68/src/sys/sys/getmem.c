/* getmem.c - getmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*------------------------------------------------------------------------
 * getmem  --  allocate heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*getmem(nbytes)
	unsigned nbytes;
{
	struct	mblock	*p, *q, *leftover;

#ifdef	DEBUG
	dotrace("getmem", &nbytes, 1);
#endif
	disable();
	if (nbytes==0 || memlist.mnext== (struct mblock *) NULL) {
		restore();
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundew(nbytes);
	for (q= &memlist,p=memlist.mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			restore();
			return( (WORD *)p );
		} else if ( p->mlen > nbytes ) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			restore();
			return( (WORD *)p );
		}
	restore();
	return( (WORD *)SYSERR );
}
