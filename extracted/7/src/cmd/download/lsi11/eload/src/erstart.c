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

	dqptr = (struct dqregs *) EDEVICE;
	/* Assumption: all reads operate on the same command list and	*/
	/* buffer.  Write operations give the deqna a read buffer (so	*/
	/* the write can update its status word).  Must verify that the	*/
	/* deqna needs a new read buffer.				*/
	if (dcmptr->dc_st1 != DC_INIT || (dqptr->d_csr & DQ_RLI) ) {
		dcmptr->dc_flag = DC_NUSED;
		dcmptr->dc_bufh = DC_VALID;
		dcmptr->dc_buf  = 0160000 | (unsigned)buf;
		dcmptr->dc_len = dqlen(len);
		dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
		dqptr->d_rcmd = 0160000 | (unsigned)dcmptr;
		dqptr->d_rcmdh = NULL;
	}
}
