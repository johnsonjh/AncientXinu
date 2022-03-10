/* ethinter.c - ethinter */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ethinter - ethernet interrupt processing procedure
 *------------------------------------------------------------------------
 */
INTPROC	ethinter(etptr)
	struct	etblk	*etptr;
{
	struct	dqregs	*dqptr;
	register struct	dcmd	*dcmptr;
	short	csr;
	int	pid;
	Bool	doresch;

	dqptr = etptr->eioaddr;
	dqptr->d_csr = csr = dqptr->d_csr;	/* clear RINT, XINT */

	/* check BOTH receive and xmit completion before resched */

	doresch = FALSE;
	if (csr & DQ_RINT) {
		dcmptr = etptr->ercmd;
		if ( (dcmptr->dc_st1 & DC_LUSE) != DC_ERRU) {
			if (proctab[etptr->etrpid].pstate == PRSUSP) {
				ready(etptr->etrpid, RESCHNO);
				doresch = TRUE;
			}
			etptr->etrpid = BADPID;
		} else {	/* error, so retry operation */
			dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
			dcmptr->dc_flag = DC_NUSED;
			dqptr->d_rcmd = (short) dcmptr;
			dqptr->d_rcmdh = (short) NULL;
		}
	}
	if (csr & DQ_XINT) {
		dcmptr = etptr->ewcmd;
		if ( (dcmptr->dc_st1 & DC_LUSE) != DC_ERRU) {
			if (etptr->etsetup == DC_NORM) {
				freebuf( dcmptr->dc_buf );
				signal(etptr->etwsem);
			}
			doresch = FALSE;
		} else if (etptr->etwtry-- > 0) { /* retry on error */
			while (! (dqptr->d_csr & DQ_XLI) )
				;
			dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
			dcmptr->dc_flag = DC_NUSED;
			dqptr->d_wcmd = (short) dcmptr;
			dqptr->d_wcmdh = (short) NULL;
		} else {
			if (etptr->etsetup == DC_NORM) {
				freebuf(dcmptr->dc_buf);
				signal(etptr->etwsem);
			}
			doresch = FALSE;
		}
	}
	if (doresch)
		resched();
}
