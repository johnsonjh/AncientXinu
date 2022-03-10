/* lfputc.c - lfputc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfputc  --  put a character onto a (buffered) disk file
 *------------------------------------------------------------------------
 */
lfputc(devptr, ch)
struct	devsw	*devptr;
char	ch;
{
	struct	flblk	*flptr;

#ifdef	DEBUG
	dotrace("lfputc", &devptr, 2);
#endif
	disable();

	flptr = (struct flblk *) devptr->dvioblk;
	if (flptr->fl_pid != currpid || !(flptr->fl_mode&FLWRITE)) {
		restore();
		return(SYSERR);
	}
	if (flptr->fl_bptr >= &flptr->fl_buff[DBUFSIZ]) {
		if (flptr->fl_dch)
			lfsflush(flptr);
		lfsetup(flptr->fl_dev, flptr);
	}
	if (flptr->fl_pos >= (flptr->fl_dent)->fdlen)
		(flptr->fl_dent)->fdlen++;
	flptr->fl_pos++;
	*(flptr->fl_bptr)++ = ch;
	flptr->fl_dch = TRUE;
	restore();
	return(OK);
}
