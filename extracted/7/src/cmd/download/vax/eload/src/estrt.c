/* estrt.c - estrt */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

/*-----------------------------------------------------------------------
 *  estrt -- initialize deqna at beginning of execution or after lock-up
 *-----------------------------------------------------------------------
*/
estrt()
{
	Eaddr	myeaddr;
	struct	dqregs	*dqptr;
	int	i, j;
	short	*iptr;
	char	setup[DQ_ROWS][DQ_COLS];
	char	secho[sizeof(setup)+4];
	struct	dcmd	ercmd[2], ewcmd[2];

	dqptr = (struct dqregs *) EDEVICE;

	/* establish read and write buffer descriptor lists */
	edcmset(&ercmd[1]);
	edcmset(&ewcmd[1]);

	/* initialize device */
	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_vect = ETHINTV;

	/* extract physical ethernet address and setup device for it	*/
	for (iptr=(short *) dqptr ,i=0 ; i<EPADLEN ; i++)
		myeaddr[i] = LOWBYTE & *iptr++;

	for (i=0 ; i < (DQ_ROWS>>1) ; i++) {
		setup[i+DQ_SETD][0] = setup[i][0] = 0;
		setup[i+DQ_SETD][1] = setup[i][1] = i<EPADLEN ? DQ_BCST : 0;
		for (j=2 ; j<DQ_COLS ; j++)
			setup[i+DQ_SETD][j] = setup[i][j] = 
				i<EPADLEN ? myeaddr[i] : 0;
	}
	erstart(ercmd, secho, sizeof(secho));
	ewstart(ewcmd, setup, sizeof(setup), DC_SETUP);

	/* poll device until setup processed */
	while (ercmd[0].dc_st1 == DC_INIT)
		; /* NULL */

	/* reset device, leaving it online */
	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_csr &= ~DQ_ELOP;
	dqptr->d_csr |= (DQ_ENBL | DQ_ILOP);
}
