/* erstart.c - erstart */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

/*------------------------------------------------------------------------
 *  erstart - start an ethernet read operation on the DEQNA
 *------------------------------------------------------------------------
 */

erstart(dcmptr, buf, len)
	struct	dcmd	*dcmptr;
	char	*buf;
	int	len;
{
	struct	dqregs	*dqptr;

	dcmptr->dc_bufh = DC_VALID;
	dcmptr->dc_buf  = 0160000 | (unsigned)buf;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr = (struct dqregs *)EDEVICE;
	dqptr->d_rcmd = 0160000 | (unsigned)dcmptr;
	dqptr->d_rcmdh = NULL;
	return;
}
