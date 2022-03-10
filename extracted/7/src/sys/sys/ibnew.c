/* ibnew.c - ibnew */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  ibnew  --  allocate a new iblock from free list on disk
 *------------------------------------------------------------------------
 */
ibnew(diskdev, writedir)
int	diskdev;
Bool	writedir;
{
	struct	dir	*dirptr;
	struct	iblk	iblock;
	IBADDR	inum;
	int	i;
	int	sem;

	sem =  ((struct dsblk *)devtab[diskdev].dvioblk)->dflsem;
	dirptr = dsdirec(diskdev);
	wait(sem);
	inum = dirptr->d_filst;
	ibget(diskdev, inum, &iblock);
	dirptr->d_filst = iblock.ib_next;
	if (writedir)
		write(diskdev, dskbcpy(dirptr), DIRBLK);
	signal(sem);
	ibclear(&iblock, 0L);
	ibput(diskdev, inum, &iblock);
	return(inum);
}
