/* mdump.c - mdump */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mem.h>

/*------------------------------------------------------------------------
 *  mdump  --  dump free memory list
 *------------------------------------------------------------------------
 */
mdump()
{
	char	ps;
	int	i;
	struct	mblock	*mptr;
	unsigned free;
	unsigned avail;
	unsigned stkmem;

	disable(ps);

	/* calculate current size of free memory and stack memory */

	for( free=0,mptr=memlist.mnext ; mptr!=(struct mblock*)NULL ;
	     mptr=mptr->mnext)
		free += mptr->mlen;
	for (stkmem=0,i=0 ; i<NPROC ; i++) {
		if (proctab[i].pstate != PRFREE)
			stkmem += (unsigned)proctab[i].pstklen;
	}
	kprintf("Memory: %u bytes real memory, %u text, %u data, %u bss\n",
		1 + (unsigned)maxaddr, (unsigned) &etext,
		(unsigned) &edata - (unsigned) &etext,
		(unsigned) &end - (unsigned) &edata);
	avail = (unsigned)maxaddr - (unsigned) &end + 1;
	kprintf(" initially: %5u avail\n", avail);
	kprintf(" presently: %5u avail, %5u stack, %5u heap\n",
		free, stkmem, avail - stkmem - free);
	kprintf(" free list:\n");
	for( mptr=memlist.mnext ; mptr!=(struct mblock *)NULL ;
	     mptr=mptr->mnext)
		kprintf("   block at %6o, length %5u (0%o)\n",
			mptr, mptr->mlen, mptr->mlen);
	restore(ps);
}
