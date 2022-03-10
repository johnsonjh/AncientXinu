/* ethrstrt.c - ethrstrt */

#include "../h/conf.h"
#include "../h/kernel.h"
#include "../h/network.h"

/*------------------------------------------------------------------------
 *  ethrstrt - start an ethernet read operation on the DEQNA
 *------------------------------------------------------------------------
 */

ethrstrt(etptr, buf, len)
	struct	etblk	*etptr;
	char	*buf;
	int	len;
{
	struct	dcmd	*dcmptr;
	struct	dqregs	*dqptr;

	dcmptr = etptr->ercmd;
	dcmptr->dc_bufh = DC_VALID;
	dcmptr->dc_buf  = buf;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr = etptr->eioaddr;
	dqptr->d_rcmd = dcmptr;
	dqptr->d_rcmdh = NULL;
	return(OK);
}
