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
	int	i;
	struct	dqsetu	setup;

	etptr = &eth[devptr->dvminor];
	devptr->dvioblk = (char *) etptr;
	iosetvec(devptr->dvnum, etptr, etptr);
	etptr->eioaddr = dqptr = (struct dqregs *) devptr->dvcsr;
	etptr->etdev = devptr;
	etptr->etrsem = screate(1);
	etptr->etwsem = screate(1);
	etptr->etrpid = 0;

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

	/* extract physical ethernet address */

	for (iptr=(short *)dqptr ,i=0 ; i<EPADLEN ; i++)
		etptr->etpaddr[i] = LOWBYTE & *iptr++;

	ethstrt(etptr, &setup);
	return(OK);
}

#ifdef	Neth
struct	etblk	eth[Neth];
#endif
