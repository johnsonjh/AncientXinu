/* ttycntl.c - ttycntl */

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
 *  ttycntl  -  control a tty device by setting modes
 *------------------------------------------------------------------------
 */
ttycntl(devptr, func, addr)
	struct	devsw	*devptr;
	int func;
	char *addr;
{
	register struct	tty *ttyp;
	register struct csr *cptr;
	char	ch;

#ifdef	DEBUG
	dotrace("ttycntl", &devptr, 3);
#endif

	ttyp = &tty[devptr->dvminor];

	cptr = realcsr(devptr);
	switch ( func )	{
	case TCSETBRK:
		slusetbrk(cptr);
		break;
	case TCRSTBRK:
		slurstbrk(cptr);
		break;
	case TCNEXTC:
		disable();
		wait(ttyp->isem);
		ch = ttyp->ibuff[ttyp->itail];
		restore();
		signal(ttyp->isem);
		return(ch);
	case TCMODER:
		ttyp->imode = IMRAW;
		break;
	case TCMODEC:
		ttyp->imode = IMCOOKED;
		break;
	case TCMODEK:
		ttyp->imode = IMCBREAK;
		break;
	case TCECHO:
		ttyp->iecho = TRUE;
		break;
	case TCNOECHO:
		ttyp->iecho = FALSE;
		break;
	case TCICHARS:
		return(scount(ttyp->isem));
	default:
		return(SYSERR);
	}
	return(OK);
}
