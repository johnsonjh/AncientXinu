/* ethwstrt.c - ethwstrt */

#include "../h/conf.h"
#include "../h/kernel.h"
#include "../h/proc.h"
#include "../h/network.h"

/*------------------------------------------------------------------------
 *  ethwstrt - start an ethernet write operation on the DEQNA
 *------------------------------------------------------------------------
 */

ethwstrt(etptr, buf, len, setup)
	struct	etblk	*etptr;
	char	*buf;
	int	len;
	int	setup;
{
	register struct	dcmd	*dcmptr;
	register struct	dqregs	*dqptr;

	blkcopy(((struct eheader *)buf)->e_src, etptr->etpaddr, EPADLEN);
	dqptr = etptr->eioaddr;
	while (! (dqptr->d_csr & DQ_XLI) )
		;
	etptr->etwpid = currpid;
	etptr->etwtry = EXRETRY;
	dcmptr = etptr->ewcmd;
	dcmptr->dc_bufh = DC_VALID | DC_ENDM | setup;
	if (isodd(len))
		dcmptr->dc_bufh |= DC_LBIT;
	dcmptr->dc_buf  = buf;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr->d_wcmd = dcmptr;
	dqptr->d_wcmdh = NULL;
}
