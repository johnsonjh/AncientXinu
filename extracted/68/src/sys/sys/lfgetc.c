/* lfgetc.c - lfgetc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfgetc  --  get next character from (buffered) disk file
 *------------------------------------------------------------------------
 */
lfgetc(devptr)
struct	devsw	*devptr;
{
	struct	flblk	*flptr;
	char	nextch;

#ifdef	DEBUG
	dotrace("lfgetc", &devptr, 1);
#endif
	disable();
	flptr = (struct flblk *)devptr->dvioblk;
	if (flptr->fl_pid!=currpid || !(flptr->fl_mode&FLREAD)) {
		restore();
		return(SYSERR);
	}
	if (flptr->fl_pos >= (flptr->fl_dent)->fdlen) {
		restore();
		return(EOF);
	}
	if (flptr->fl_bptr >= &flptr->fl_buff[DBUFSIZ]) {
		if (flptr->fl_dch)
			lfsflush(flptr);
		lfsetup(flptr->fl_dev, flptr);
	}
	nextch = *(flptr->fl_bptr)++;
	flptr->fl_pos++;
	restore();
	return(nextch);
}
