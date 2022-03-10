/* dumkfs.c - dumkfs, dumkdl, dumkil */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  dumkfs  --  make a fresh file system on a disk
 *------------------------------------------------------------------------
 */
dumkfs(diskdev, niblks, nblocks, diskid, verbose)
int	diskdev;
int	niblks;
unsigned nblocks;
int	diskid;
Bool	verbose;
{
	DBADDR	firstdb, lastdb;
	struct	dir	*dirptr;

	if (verbose)
		kprintf("Clearing directory block...\n");
	dirptr = dsdirec(diskdev);
	firstdb = (DBADDR) (ibtodb(niblks-1) + 1);
	lastdb = (DBADDR) (nblocks - 1);
	dirptr->d_iblks = niblks;
	dirptr->d_fblst = firstdb;
	dirptr->d_filst = (IBADDR) niblks-1;
	dirptr->d_id = diskid;
	dirptr->d_nfiles = 0;
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	dumkil(diskdev, niblks, verbose);
	dumkdl(diskdev, firstdb, lastdb, verbose);	
	if (verbose)
		kprintf("Done initializing disk...\n");
	return(OK);
}


/*------------------------------------------------------------------------
 *  dumkdl  --  link all data blocks into free list to init. file system
 *------------------------------------------------------------------------
 */
dumkdl(diskdev, firstfb, lastfb, verbose)
int	diskdev;
DBADDR	firstfb, lastfb;
Bool	verbose;
{
	struct	freeblk	*buff;
	DBADDR	dba;
	char	*bptr;
	int	i;

	if (verbose)
		kprintf("Linking %u free blocks from %u through %u...\n",
			lastfb - firstfb + (DBADDR)1, firstfb, lastfb);
	bptr = buff = (struct freeblk *) getbuf(dskdbp);
	for (i=0 ; i<DBUFSIZ ; i++)
		*bptr++ = '\000';
	for (dba=firstfb ; dba < lastfb ; dba++) {
		buff->fbnext = (DBADDR) (dba+1);
		write(diskdev, dskbcpy(buff), dba);
		if (verbose && (dba%500 == 0))
			kprintf("%5u\r",dba);
	}
	buff->fbnext = (DBADDR) DIRBLK;
	write(diskdev, buff, dba);
	if (verbose)
		kprintf("\nDone linking data blocks...\n");
	return(OK);
}



/*------------------------------------------------------------------------
 *  dumkil  --  link ALL i-blocks onto a free list
 *------------------------------------------------------------------------
 */
dumkil(diskdev, niblks, verbose)
int	diskdev;
int	niblks;
Bool	verbose;
{
	struct	iblk	iblock;
	IBADDR	ibnum;
	int	i;

	if (verbose)
		kprintf("Linking %d i-blocks together...\n", niblks);
	ibclear(&iblock, 0L);
	for (ibnum=niblks-1 ; ibnum>0 ; ibnum--) {
		iblock.ib_next = ibnum-1;
		ibput(diskdev, ibnum, &iblock);
	}
	iblock.ib_next = IBNULL;
	ibput(diskdev, 0, &iblock);
	if (verbose)
		kprintf("Done linking i-blocks...\n");
	return(OK);
}
