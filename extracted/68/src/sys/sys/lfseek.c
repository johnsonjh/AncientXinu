/* lfseek.c - lfseek */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfseek  --  seek to a specified position of a file
 *------------------------------------------------------------------------
 */
lfseek(devptr, offset)
struct	devsw	*devptr;
long	offset;
{
	struct	flblk	*flptr;
	int	retcode;

#ifdef	DEBUG
	dotrace("lfseek", &devptr, 2);
#endif
	disable();
	flptr = (struct flblk *)devptr->dvioblk;
	if (flptr->fl_mode & FLWRITE) {
		if (flptr->fl_dch)
			lfsflush(flptr);
	} else if (offset > (flptr->fl_dent)->fdlen) {
			restore();
			return(SYSERR);
	}
	flptr->fl_pos = offset;
	retcode = lfsetup(flptr->fl_dev, flptr);
	restore();
	return(retcode);
}
