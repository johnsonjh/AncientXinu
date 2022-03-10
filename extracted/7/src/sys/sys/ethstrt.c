/* ethstrt.c - ethstrt.c */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ethstrt  -  reset DEQNA Ethernet hardware and leave it operating
 *------------------------------------------------------------------------
 */
ethstrt(etptr, setup)
struct	etblk	*etptr;
struct	dqsetup	*setup;
{
	struct	dqregs	*dqptr;
	struct	dcmd	*dcmptr;
	int	i, j;

	dqptr = etptr->eioaddr;

	/* initialize device */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_vect = (etptr->etdev)->dvivec;

	/* build setup packet */

	for (i=0 ; i < (DQ_ROWS>>1) ; i++) {
		setup->dq_set[i+DQ_SETD][0] = setup->dq_set[i][0] = 0;
		setup->dq_set[i+DQ_SETD][1] = setup->dq_set[i][1] =
			i<EPADLEN ? DQ_BCST : 0;
		for (j=2 ; j<DQ_COLS ; j++)
			setup->dq_set[i+DQ_SETD][j] = setup->dq_set[i][j]=
				i<EPADLEN ? etptr->etpaddr[i] : 0;
	}
	ethrstrt(etptr, setup->dq_echo,
		sizeof(setup->dq_echo) + sizeof(setup->dq_crc) );
	ethwstrt(etptr, setup->dq_set, sizeof(setup->dq_set), DC_SETUP);

	/* poll device until setup processed */

	for (dcmptr=etptr->ercmd ; dcmptr->dc_st1 == DC_INIT ; )
		;

	/* reset device, leaving it online */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_csr &= ~DQ_ELOP;
	dqptr->d_csr |= (DQ_ENBL| DQ_IEN | DQ_ILOP);
	return(OK);
}
