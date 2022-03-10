/* ttyputc.c - ttyputc */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <slu.h>

/*------------------------------------------------------------------------
 *  ttyputc - write one character to a tty device
 *------------------------------------------------------------------------
 */
ttyputc(devptr, ch )
struct	devsw	*devptr;
char	ch;
{
	struct	tty   *iptr;
	char	ps;

	iptr = &tty[devptr->dvminor];
        if ( ch==NEWLINE && iptr->ocrlf )
                ttyputc(devptr,RETURN);
	disable(ps);
	wait(iptr->osem);		/* wait	for space in queue	*/
	iptr->obuff[iptr->ohead++] = ch;
	if (iptr->ohead	>= OBUFLEN)
		iptr->ohead = 0;
	(iptr->ioaddr)->ctstat = SLUENABLE;
	restore(ps);
	return(OK);
}
