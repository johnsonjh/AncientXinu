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
	int	i, j, err;

	/* we perform timeout on reads, to catch deqna lockup problem	*/
	/* the loop constants below are configured for an LSI11/23	*/
	/* and should be modified for other LSI models			*/
	while (1) {
	   erstart(dcmptr, buf, len);		/* start read operation */
	   err = FALSE;
	   /* simulate a 2-sec timeout */
	   for (j = 0; j < 3 && !err; j++)
		for (i = 0; i < 20000; i++)
			if (dcmptr->dc_st1 != DC_INIT) {
				if ( (dcmptr->dc_st1 & DC_LUSE) != DC_ERRU) {
					len = (dcmptr->dc_st1 & DC_HLEN) |
						(dcmptr->dc_st2 & DC_LLEN);
					len += DC_XLEN;
					return(len);
				} else {
					err = TRUE;
					break;
				}
			}
	   estrt();		/* device locked up?	*/
	}
}
