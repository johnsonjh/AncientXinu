/* eread.c - eread */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

/*------------------------------------------------------------------------
 *  eread - read a single packet from the ethernet
 *------------------------------------------------------------------------
 */
eread(dcmptr, buf, len)
struct	dcmd	*dcmptr;
char	*buf;
int	len;
{
	erstart(dcmptr, buf, len);
	while(dcmptr->dc_st1 == DC_INIT)
		;
	if ( (dcmptr->dc_st1 & DC_LUSE) == DC_ERRU) {
		return(SYSERR);
	}
	len = (dcmptr->dc_st1 & DC_HLEN) | (dcmptr->dc_st2 & DC_LLEN);
	len += DC_XLEN;
	return(len);
}
