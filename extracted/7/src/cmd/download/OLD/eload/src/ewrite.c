/* ewrite.c - ewrite */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

/*------------------------------------------------------------------------
 *  ewrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */
ewrite(dcmptr, myeaddr, packet, len)
struct	dcmd	*dcmptr;
Eaddr	*myeaddr;
struct	epacket	*packet;
int	len;
{
	if (len < EMINPAK)
		len = EMINPAK;
	blkcopy(packet->ep_hdr.e_src, myeaddr, EPADLEN);
	ewstart(dcmptr, packet, len, DC_NORM);
}
