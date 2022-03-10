/* ethinit.c - ethinit */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ethinit  -  initialize ethernet I/O device and buffers
 *------------------------------------------------------------------------
 */
ethinit(devptr)
	struct	devsw	*devptr;
{
	struct	etblk	*etptr;
	struct	dqregs	*dqptr;
	struct	dcmd	*dcmptr;
	short	*iptr;
	int	i, j;
	char	setup[DQ_ROWS][DQ_COLS];
	char	secho[sizeof(setup)+4];

	etptr = &eth[devptr->dvminor];
	devptr->dvioblk = (char *) etptr;
	iosetvec(devptr->dvnum, etptr, etptr);
	etptr->eioaddr = dqptr = (struct dqregs *) devptr->dvcsr;
	etptr->etrsem = screate(1);
	etptr->etwsem = screate(1);
	etptr->etrpid = etptr->etwpid = 0;

	/* establish read and write buffer descriptor lists */

	dcmptr = &etptr->ercmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = (short) NULL;

	dcmptr = &etptr->ewcmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = (short) NULL;
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;

	/* initialize device */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_vect = devptr->dvivec;

	/* extract physical ethernet address and setup device for it	*/

	for (iptr=(short *)dqptr ,i=0 ; i<EPADLEN ; i++)
		etptr->etpaddr[i] = LOWBYTE & *iptr++;

	for (i=0 ; i < (DQ_ROWS>>1) ; i++) {
		setup[i+DQ_SETD][0] = setup[i][0] = 0;
		setup[i+DQ_SETD][1] = setup[i][1] = i<EPADLEN ? DQ_BCST : 0;
		for (j=2 ; j<DQ_COLS ; j++)
			setup[i+DQ_SETD][j] = setup[i][j] = 
				i<EPADLEN ? etptr->etpaddr[i] : 0;
	}
	ethrstrt(etptr, secho, sizeof(secho));
	ethwstrt(etptr, setup, sizeof(setup), DC_SETUP);

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

#ifdef	Neth
struct	etblk	eth[Neth];
#endif
