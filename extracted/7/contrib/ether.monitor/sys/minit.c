/* minit.c - minit */

#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <slu.h>
#include "../h/network.h"

#define	CDEVICE		0177560		/* console device address        */
#define ETHINTV		0400		/* DEQNA interrupt vector        */
#define	EDEVICE	0174440
#define	kputc(x,c)	while((x->ctstat & SLUREADY) == 0); x->ctbuf = c


minit()
{

	struct	dqregs	*dqptr;		/* pointer to DEQNA registers	*/
	struct	dcmd	*dcmptr;	/* pointer to DEQNA command list*/
	struct	csr	*console;	/* console line CSR address	*/
	int	*iptr;			/* integer pointer		*/
	int	i, j, len;		/* indexes and length of input	*/
	char	setup[DQ_ROWS][DQ_COLS];/* area for setup packet	*/
	char	secho[sizeof(setup)+4];	/* area for setup packet echo	*/
	char	ps;
	struct	dcmd	ercmd[2];	/* DEQNA read command list	*/
	struct	dcmd	ewcmd[2];	/* DEQNA write command list	*/
	Eaddr	myeaddr;		/* my Ethernet address		*/

	/* set up device addresses */


	dqptr = (struct dqregs *) EDEVICE;
	console = (struct csr *)  CDEVICE;


	/* establish read and write buffer descriptor lists */

	disable(ps);
	dcmptr = &ercmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = NULL;
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;

	dcmptr = &ewcmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = NULL;
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;

	/* initialize device */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_vect = ETHINTV;

	/* extract physical ethernet address and setup device for it	*/

	for (iptr=(int *)dqptr ,i=0 ; i<EPADLEN ; i++)
		myeaddr[i] = LOWBYTE & *iptr++;

	for (i=0 ; i < (DQ_ROWS>>1) ; i++) {
		setup[i+DQ_SETD][0] = setup[i][0] = 0;
		setup[i+DQ_SETD][1] = setup[i][1] = i<EPADLEN ? DQ_BCST : 0;
		for (j=2 ; j<DQ_COLS ; j++)
			setup[i+DQ_SETD][j] = setup[i][j] = 
				i<EPADLEN ? myeaddr[i] : 0;
	}
	erstart(&ercmd[0], secho, sizeof(secho));
	ewstart(&ewcmd[0], setup, sizeof(setup)+2, DC_SETUP);
	for(dcmptr=ewcmd; dcmptr->dc_st1 == DC_INIT;){
		continue;
	}
	if((dcmptr->dc_st1 & DC_LUSE) == DC_ERRU){
		fprintf(0,"DC_ERRU!\n");
	}

	/* poll device until setup processed */

	for (dcmptr=ercmd ; dcmptr->dc_st1 == DC_INIT ; )
		;
	if((dcmptr->dc_st1 & DC_LUSE) == DC_ERRU){
		fprintf(0,"Eread is DC_ERRU!\n");
	}
	/* reset device, leaving it online */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_csr &= ~DQ_IEN;
	dqptr->d_csr &= ~DQ_ELOP;
	dqptr->d_csr |= (DQ_ENBL| DQ_ILOP);
	restore(ps);
	return(OK);
}
