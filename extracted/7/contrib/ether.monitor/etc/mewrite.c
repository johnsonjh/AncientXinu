/* mewrite.c - mewrite */

#include <kernel.h>
#include <deqna.h>
#include <ether.h>

#define	EDEVICE	0174440
/*------------------------------------------------------------------------
 *  mewrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */
mewrite(dcmptr, myeaddr, packet, len)
struct	dcmd	*dcmptr;
Eaddr	*myeaddr;
struct	epacket	*packet;
int	len;
{
	if (len < EMINPAK)
		len = EMINPAK;
	ewstart(dcmptr, packet, len, DC_NORM);
	if((dcmptr->dc_st1 & DC_LUSE) == DC_ERRU){
		fprintf(0,"DC_ERRU\n");
	}
}

/*-----------------------------------------------------------------------------
 *  ewstart - start an ethernet write operation on the DEQNA
 *-----------------------------------------------------------------------------
 */

ewstart(dcmptr, packet, len, setup)
	struct	dcmd	*dcmptr;
	struct	epacket	*packet;
	int	len;
	int	setup;
{
	register struct	dqregs	*dqptr;

	dqptr = (struct dqregs *)EDEVICE;
	while (! (dqptr->d_csr & DQ_XLI) )
		fprintf(0,"E");
	dcmptr->dc_bufh = DC_VALID | DC_ENDM | setup;
	if (isodd(len))
		dcmptr->dc_bufh |= DC_LBIT;
	dcmptr->dc_buf  = (unsigned)packet;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr->d_wcmd = (unsigned)dcmptr;
	dqptr->d_wcmdh = NULL;
	return;
}
