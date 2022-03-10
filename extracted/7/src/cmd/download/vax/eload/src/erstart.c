/* erstart.c - erstart */

#include <eload.h>
#include <deqna.h>
#include <ether.h>
#include <qbus.h>

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
		dcmptr->dc_bufh = DC_VALID | hiQ2low(buf);
		dcmptr->dc_buf  = (short) buf;
		dcmptr->dc_len = dqlen(len);
		dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
		dqptr->d_rcmd = (short)dcmptr;
		dqptr->d_rcmdh = hiQ2low(dcmptr);
	}
}
