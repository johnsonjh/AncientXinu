/* udpipwr.c -- udpipwr */

#include "../h/deqna.h"
#include "../h/ether.h"
#include "../h/ip.h"
#include "../h/udp.h"
#include "../h/eload.h"

/*---------------------------------------------------------------------------
 * udpipwr -- write a udp packet over the ethernet
 *---------------------------------------------------------------------------
*/
udpipwr(myeaddr, idrecvd, wpkt, wcmd, wpktlen, rpkt, rcmd, rpktlen)
Eaddr	myeaddr;
Bool	idrecvd;
struct	epacket	*wpkt, *rpkt;
struct	dcmd	*wcmd, *rcmd;
int	wpktlen, rpktlen;
{
	register struct	ip *ipptr;

	if (idrecvd) {
	    ipptr = (struct ip *) wpkt->ep_data;
	    ipptr->i_paclen = hs2net(wpktlen-EHLEN);
	    ipptr->i_id = hs2net(net2hs(ipptr->i_id) + 1);
	    ipptr->i_cksum = 0;
	    ipptr->i_cksum = cksum(ipptr, IPHLEN>>1);
	    ((struct udp *)(ipptr->i_data))->u_udplen =
						  hs2net(wpktlen-EHLEN-IPHLEN);
	    ewrite(myeaddr, wcmd, wpkt, wpktlen, rcmd, rpkt, rpktlen);
	 }
}
