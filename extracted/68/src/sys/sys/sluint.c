/* sluint.c - sluint */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <io.h>
#include <necuart.h>

#define A	0
#define B	1

/*------------------------------------------------------------------------
 *  sluint  --  interrupt dispatcher for a NEC 7201 SLU
 *------------------------------------------------------------------------
 */
INTPROC	sluint(ttyp)
	register struct	tty	*ttyp;
{
	register struct	csr	*cptr;
	register int	inttype;		/* Interrupt Type	*/
	register int	channel;		/* Interrupt Channel	*/
	register struct	csr *chanAptr, *chanBptr;
	int	error;

	cptr = chanAptr = ttyp->ioaddr;		/* Chan A csr pointer	*/
	chanBptr = chanAptr + 1;
	chanBptr->crtstat =  2;			/* Point to Reg. 2B	*/ 
	inttype = chanBptr->crtstat;		/* Read Interrupt type	*/
	channel = inttype & NECchan ? A : B;	/* Determine channel	*/
	if (channel == B) {
		cptr++;
		ttyp++;
	}
	inttype &= ~NECchan;			
	switch (inttype) {
	    case NECtxclear:			/* Tx Buffer Empty	*/
		cptr->crtstat=NECtxires;
		ttyoin(ttyp, cptr);
		break;
	    case NECreceive:			/* Received Character	*/
		ttyiin(ttyp, cptr, FALSE);
		chanAptr->crtstat = NECeoi;	/* End of interrupt*/
		break;
	    case NECspecial:			/* Receive with Error	*/
		cptr->crtstat = RD1;
		error = cptr->crtstat & NECerror;
		cptr->crtstat=NECerres;		/* Error Reset		*/
		ttyiin(ttyp, cptr, error);
		chanAptr->crtstat = NECeoi;	/* End of interrupt*/
		break;
	    case NECextern:			/* External Change	*/
		cptr->crtstat=NECextres;
		chanAptr->crtstat=NECeoi;	/* End of interrupt	*/
		kprintf("External/Status Change Interrupt!\n");
		break;
	    default:
		kprintf("Extraneous interrupt, Type: 0x%x\n",inttype);
		panic();
		break;
	}
	return(OK);
}
