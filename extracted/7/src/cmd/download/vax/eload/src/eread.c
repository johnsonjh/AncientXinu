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

	while (1) {
	   err = FALSE;
	   erstart(dcmptr, buf, len);		/* start read operation */
	   /* simulate ~2 second timeout (HACK!!) */
	   for (j = 0; j < 9 && !err; j++)
		for (i = 0; i < 30000; i++) {
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
		}
	   estrt();		/* device locked up?	*/
	}
}
