/* lfsnewd.c - lfsnewd */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

#define  DFILLER  '+'

/*------------------------------------------------------------------------
 *  lfsnewd  --  allocate a new data block from free list on disk
 *------------------------------------------------------------------------
 */
lfsnewd(diskdev, flptr)
int	diskdev;
struct	flblk	*flptr;
{
	struct	dir	*dirptr;
	struct	freeblk	*fbptr;
	char	*buf;
	int	sem;
	DBADDR	dba;
	int	i;

	dirptr = (struct dir *) dsdirec(diskdev);
	fbptr = (struct freeblk *) (buf = flptr->fl_buff);
	sem = ((struct dsblk *)devtab[diskdev].dvioblk)->dflsem; 
	wait(sem);
	dba = dirptr->d_fblst;
	read(diskdev, fbptr, dba);
	dirptr->d_fblst = fbptr->fbnext;
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	signal(sem);
	for (i=0 ; i<DBUFSIZ ; i++)
		*buf++ = DFILLER;
	write(diskdev, dskbcpy(fbptr), dba);
	return(dba);
}
