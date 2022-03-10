/* ibfree.c - ibfree */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  ibfree.c  --  deallocate an iblock and return to the free list
 *------------------------------------------------------------------------
 */
ibfree(diskdev, inum)
int	diskdev;
IBADDR	inum;
{
	struct	dir	*dirptr;
	struct	iblk	*ibptr;
	char	*buff;
	DBADDR	dba;
	int	i, sem;

	dirptr = dsdirec(diskdev);
	buff = getbuf(dskdbp);
	dba = ibtodb(inum);
	read(diskdev, buff, dba);
	ibptr = (struct iblk *)(buff + ibdisp(inum));
	for (i=0 ; i<IBLEN ; i++)
		if ( (dba=ibptr->ib_dba[i]) != DBNULL)
			lfsdfree(diskdev, dba);
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk )->dflsem;
	wait(sem);
	ibptr->ib_next = dirptr->d_filst;
	dirptr->d_filst = inum;
	write(diskdev, buff, dba);
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	signal(sem);
	return(OK);
}
