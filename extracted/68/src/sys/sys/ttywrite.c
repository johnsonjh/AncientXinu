/* ttywrite.c - ttywrite, writcopy */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>

#ifdef	lsi11
#	include <slu.h>
#else
#	include <necuart.h>
#endif

/*------------------------------------------------------------------------
 *  ttywrite - write one or more characters to a tty device
 *------------------------------------------------------------------------
 */
ttywrite(devptr, buff, count)
	register struct	devsw	*devptr;
	register char	*buff;
	register int	count;
{
	register struct csr *cptr;
	register struct tty *ttyp;
	int avail;

#ifdef	DEBUG
	kprintf("ttywrite(%x, %x, %x)\n", devptr, buff, count);
#endif
	ttyp = &tty[devptr->dvminor];
	cptr = realcsr(devptr);

	disable();
	if ( (avail=scount(ttyp->osem)) > count) {
		writcopy(buff, ttyp, count);
	} else {
		writcopy(buff, ttyp, avail);
		buff += avail;
		for (count-=avail ; count>0 ; count--)
			ttyputc(devptr, *buff++);
	}
#ifdef	mc68000
	if (!ttyp->tbusy)
		ttyoin(ttyp, cptr);
#endif
	restore();
	return(OK);
}

/*------------------------------------------------------------------------
 *  writcopy - high-speed copy from user's buffer into system buffer
 *------------------------------------------------------------------------
 */
LOCAL writcopy(buff, ttyp, count)
	register char	*buff;
	register struct	tty *ttyp;
	register int	count;
{
	register char	*qhead, *qend, *uend;

	qhead = &ttyp->obuff[ttyp->ohead];
	qend  = &ttyp->obuff[OBUFLEN];
	uend  = buff + count;
	while (buff < uend) {
		*qhead++ = *buff++;
		if ( qhead >= qend )
			qhead = ttyp->obuff;
	}
	ttyp->ohead = qhead - ttyp->obuff;
	sreset(ttyp->osem, scount(ttyp->osem)-count);
}
