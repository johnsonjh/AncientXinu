/* getmem.c - getmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>

/*------------------------------------------------------------------------
 * getmem  --  allocate heap storage, returning lowest integer address
 *------------------------------------------------------------------------
 */
SYSCALL	*getmem(nbytes)
	unsigned nbytes;
{
	char	ps;
	struct	mblock	*p, *q, *leftover;

	disable(ps);
	if (nbytes==0 || memlist.mnext==NULL) {
		restore(ps);
		return( (int *)SYSERR);
	}
	nbytes = (unsigned) roundew(nbytes);
	for (q= &memlist,p=memlist.mnext ; p!=NULL ; q=p,p=p->mnext)
		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			restore(ps);
			return( (int *)p );
		} else if ( p->mlen > nbytes ) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			restore(ps);
			return( (int *)p );
		}
	restore(ps);
	return( (int *)SYSERR );
}
