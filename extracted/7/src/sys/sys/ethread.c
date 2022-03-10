/* ethread.c - ethread */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  ethread - read a single packet from the ethernet
 *------------------------------------------------------------------------
 */
ethread(devptr, buff, len)
struct	devsw	*devptr;
char	*buff;
int	len;
{
	char	ps;
	char	*wbuff;
	struct	etblk	*etptr;
	struct	dcmd	*dcmptr;
	struct	dqregs	*dqptr;

	etptr = (struct etblk *) devptr->dvioblk;
	dcmptr = etptr->ercmd;
	dqptr = etptr->eioaddr;
	wait(etptr->etrsem);
	disable(ps);
	etptr->etrpid = currpid;
	recvclr();
	ethrstrt(etptr, buff, len);
	while (recvtim(DQ_RTO) == TIMEOUT) {
		wbuff = (dcmptr=etptr->ewcmd)->dc_buf;
		ethstrt(etptr, buff);
		ethrstrt(etptr, buff, len);
		if (etptr->etlen != 0) {
			ethwstrt(etptr, wbuff, etptr->etlen, DC_NORM);
		}
	}
	if ( (dcmptr->dc_st1 & DC_LUSE) == DC_ERRU) {
		len = SYSERR;
	} else {
		len = (dcmptr->dc_st1&DC_HLEN)|(dcmptr->dc_st2 & DC_LLEN);
		len += DC_XLEN;
	}
	signal(etptr->etrsem);
	restore(ps);
	return(len);
}
