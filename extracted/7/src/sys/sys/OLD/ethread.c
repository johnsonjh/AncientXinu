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
	struct	etblk	*etptr;
	struct	dcmd	*dcmptr;

	etptr = (struct etblk *) devptr->dvioblk;
	wait(etptr->etrsem);
	disable(ps);
	etptr->etrpid = currpid;
	ethrstrt(etptr, buff, len);
	suspend(currpid);
	dcmptr = etptr->ercmd;
	if ( (dcmptr->dc_st1 & DC_LUSE) == DC_ERRU) {
		signal(etptr->etrsem);
		restore(ps);
		return(SYSERR);
	}
	len = (dcmptr->dc_st1 & DC_HLEN) | (dcmptr->dc_st2 & DC_LLEN);
	len += DC_XLEN;
	signal(etptr->etrsem);
	restore(ps);
	return(len);
}
