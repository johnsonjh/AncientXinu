/* ttyputc.c - ttyputc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <io.h>

#ifdef	lsi11
#	include <slu.h>
#else
#	include <necuart.h>
#endif

/*------------------------------------------------------------------------
 *  ttyputc - write one character to a tty device
 *------------------------------------------------------------------------
 */
ttyputc(devptr, ch )
	struct	devsw	*devptr;
	char	ch;
{
	register struct	tty	*ttyp;
	register struct	csr	*cptr;

#ifdef	DEBUG
	dotrace("ttyputc", &devptr, 2);
#endif
	ttyp = &tty[devptr->dvminor];

	cptr = realcsr(devptr);
        if ( ch==NEWLINE && ttyp->ocrlf )
                ttyputc(devptr,RETURN);
	wait(ttyp->osem);			/* wait	for space in queue*/
	disable();
	ttyp->obuff[ttyp->ohead++] = ch;
	if (ttyp->ohead	>= OBUFLEN)
		ttyp->ohead = 0;
#ifdef	mc68000
	if (!ttyp->tbusy)
		ttyoin(ttyp, cptr);
#endif
	restore();
	return(OK);
}
