/* lfsdfree.c - lfsdfree */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfsdfree  --  free a data block given its address
 *------------------------------------------------------------------------
 */
lfsdfree(diskdev, dba)
int	diskdev;
DBADDR	dba;
{
	struct	dir	*dirptr;
	int	dirsem;
	struct	freeblk	*buf;

#ifdef	DEBUG
	dotrace("lfsdfree", &diskdev, 2);
#endif
	dirptr = dsdirec(diskdev);
	dirsem = devtab[diskdev].dvioblk->dflsem;
	buf = (struct freeblk *)getbuf(dskdbp);
	wait(dirsem);
	buf->fbnext = dirptr->d_fblst;
	dirptr->d_fblst = dba;
	write(diskdev, buf, dba);
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	signal(dirsem);
	return(OK);
}
