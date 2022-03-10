/* meread.c - meread */

#include <kernel.h>
#include <conf.h>
#include <deqna.h>
#include <ether.h>

#define	EDEVICE	0174440;
/*------------------------------------------------------------------------
 *  meread - read a single packet from the ethernet
 *------------------------------------------------------------------------
 */
meread(devptr, dcmptr, buf, len)
struct	devsw	*devptr;
struct	dcmd	*dcmptr;
char	*buf;
int	len;
{
	int	count;
	struct	dqregs	*dqptr;
	struct	etblk	*etptr;


	dqptr = (struct dqregs*) EDEVICE;
	etptr = (struct etptr *) devptr->dvioblk;
	wait(etptr->etrsem);

	erstart(dcmptr, buf, len);
	for(count=0; count<2000 && dcmptr->dc_st1==DC_INIT; count++){
		continue;
	}
	if(count == 2000)
		return(SYSERR);
	if ( (dcmptr->dc_st1 & DC_LUSE) == DC_ERRU) {
		return(SYSERR);
	}
	len = (dcmptr->dc_st1 & DC_HLEN) | (dcmptr->dc_st2 & DC_LLEN);
	len += DC_XLEN;
	signal(etptr->etrsem);
	return(len);
}

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
	char	ps;

	disable(ps);
	dcmptr->dc_bufh = DC_VALID;
	dcmptr->dc_buf = (unsigned)buf;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr = (struct dqregs *)EDEVICE;
	dqptr->d_rcmd = (unsigned)dcmptr;
	dqptr->d_rcmdh = NULL;
	restore(ps);
	return(OK);
}
