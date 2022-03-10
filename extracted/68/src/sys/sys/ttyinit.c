/* ttyinit.c - ttyinit */

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
 *  ttyinit - initialize buffers and modes for a tty line
 *------------------------------------------------------------------------
 */
ttyinit(devptr)
	struct	devsw	*devptr;
{
	register struct	tty *ttyp;
	int	isconsole;

#ifdef	DEBUG
	dotrace("ttyinit", &devptr, 1);
#endif

	ttyp = &tty[devptr->dvminor];

#ifdef	mc68000
	/* set up interrupt vector and interrupt dispatch table */
	if (isodd(devptr->dvminor))
		iosetvec(devptr->dvnum, ttyp - 1, ttyp - 1);
	else
#endif
		iosetvec(devptr->dvnum, ttyp, ttyp);

	devptr->dvioblk = (char *) ttyp;	/* fill tty control blk	*/
	isconsole = (devptr->dvnum == CONSOLE);	/* make console cooked	*/
	ttyp->ioaddr = devptr->dvcsr;		/* copy in csr address	*/
	ttyp->ihead = ttyp->itail = 0;		/* empty input queue	*/
	ttyp->isem = screate(0);		/* chars. read so far=0	*/
	ttyp->osem = screate(OBUFLEN);		/* buffer available=all */
	ttyp->odsend = 0;			/* sends delayed so far	*/
	ttyp->ohead = ttyp->otail = 0;		/* output queue empty	*/
	ttyp->ehead = ttyp->etail = 0;		/* echo queue empty	*/
	ttyp->imode = (isconsole ? IMCOOKED : IMRAW);
	ttyp->iecho = ttyp->evis = isconsole;	/* echo console input	*/
	ttyp->ierase = ttyp->ieback = isconsole;/* console honors erase	*/
	ttyp->ierasec = BACKSP;			/*  using ^h		*/
	ttyp->ecrlf = ttyp->icrlf = isconsole;	/* map RETURN on input	*/
	ttyp->ocrlf = ttyp->oflow = isconsole;
	ttyp->ikill = isconsole;		/* set line kill == @	*/
	ttyp->ikillc = ATSIGN;
	ttyp->oheld = FALSE;
	ttyp->ostart = STRTCH;
	ttyp->ostop = STOPCH;
	ttyp->icursor = 0;
	ttyp->ifullc = TFULLC;
	ttyp->tbusy = FALSE;
#ifdef	mc68000
	ttyp->intrstate = NECcond;		/* Cond. Affects Vector	*/
#endif
	sluinit(devptr, ttyp);
}
