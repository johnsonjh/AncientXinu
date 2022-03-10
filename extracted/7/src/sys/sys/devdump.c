/* devdump.c - devdump */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  devdump  -  dump device names from the device switch table
 *------------------------------------------------------------------------
 */
devdump()
{
	struct	devsw	*devptr;
	int	i;

	kprintf("Num  Device   minor   CSR   i-vect. o-vect. cntrl blk\n");
	kprintf("--- --------  ----- ------- ------- ------- ---------\n");
	for (i=0 ; i<NDEVS ; i++) {
		devptr = &devtab[i];
		kprintf("%2d. %-9s %3d   %07o %07o %07o  %07o\n",
			i, devptr->dvname, devptr->dvminor,
			devptr->dvcsr, devptr->dvivec, devptr->dvovec,
			devptr->dvioblk);
	}
}
