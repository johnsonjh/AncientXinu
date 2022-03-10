/* bpdump.c - bpdump */

#include <conf.h>
#include <kernel.h>
#include <mark.h>
#include <bufpool.h>

/*------------------------------------------------------------------------
 *  bpdump  -  dump the buffer pool table
 *------------------------------------------------------------------------
 */
bpdump()
{
	int	i;
	struct	bpool	*bpptr;

	for (i=0 ; i<NBPOOLS ; i++) {
	    bpptr = &bptab[i];
	    kprintf("Buffer Pool %2d: buf size=%4d, sem=%2d, count=%d\n",
		i, bpptr->bpsize, bpptr->bpsem, scount(bpptr->bpsem));
	}
	return(OK);
}
