/* ewstart.c - ewstart */

#include <eload.h>
#include <deqna.h>
#include <ether.h>
#include <qbus.h>

ewstart(dcmptr, packet, len, setup)
	struct	dcmd	*dcmptr;
	struct	epacket	*packet;
	int	len;
	int	setup;
{
	register struct	dqregs	*dqptr;

	dqptr = (struct dqregs *) EDEVICE;
	dcmptr->dc_bufh = DC_VALID | DC_ENDM | setup | hiQ2low(packet);
	if (isodd(len))
		dcmptr->dc_bufh |= DC_LBIT;
	dcmptr->dc_buf  = (short) packet;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr->d_wcmd = (short) dcmptr;
	dqptr->d_wcmdh = hiQ2low(dcmptr);
}
