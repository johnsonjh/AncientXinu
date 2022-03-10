/* lfclose.c - lfclose */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  lfclose  --  close a file by flushing output and freeing device slot
 *------------------------------------------------------------------------
 */
lfclose(devptr)
struct	devsw	*devptr;
{
	struct	dsblk	*dsptr;
	struct	dir	*dirptr;
	struct	flblk	*flptr;
	int	diskdev;

#ifdef	DEBUG
	dotrace("lfclose", &devptr, 1);
#endif
	disable();
	flptr = (struct flblk *) devptr->dvioblk;
	if (flptr->fl_pid != currpid) {
		restore();
		return(SYSERR);
	}
	diskdev = flptr->fl_dev;
	dsptr = (struct dsblk *)devtab[diskdev].dvioblk;
	dirptr = (struct dir *) dsptr->ddir;
	if ( (flptr->fl_mode & FLWRITE) && flptr->fl_dch)
		lfsflush(flptr);
	flptr->fl_pid = NULLPROC;
	dsptr->dnfiles--;
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	restore();
	return(OK);
}

