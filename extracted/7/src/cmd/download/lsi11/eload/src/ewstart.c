/* ewstart.c - ewstart */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

ewstart(dcmptr, packet, len, setup)
	struct	dcmd	*dcmptr;
	struct	epacket	*packet;
	int	len;
	int	setup;
{
	register struct	dqregs	*dqptr;

	dqptr = (struct dqregs *) EDEVICE;
	dcmptr->dc_bufh = DC_VALID | DC_ENDM | setup;
	if (isodd(len))
		dcmptr->dc_bufh |= DC_LBIT;
	dcmptr->dc_buf  = 0160000 | (unsigned)packet;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr->d_wcmd = 0160000 | (unsigned)dcmptr;
	dqptr->d_wcmdh = NULL;
}
