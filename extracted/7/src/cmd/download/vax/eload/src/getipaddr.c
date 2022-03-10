/* getipaddr.c - getipaddr */

#include "../ethdown.h"
#include "../h/eload.h"
#include "../h/ether.h"
#include "../h/ip.h"
#include "../h/arp.h"

/*------------------------------------------------------------------------
 *  getipaddr  -  send a RARP request
 *------------------------------------------------------------------------
 */
getipaddr(myipaddr, wpkt, rpkt, myeaddr, ewcmd, ercmd)
IPaddr	myipaddr;
struct	epacket	*wpkt, *rpkt;
Eaddr	myeaddr;
struct	dcmd	*ewcmd, *ercmd;
{

#ifndef	RARP
	/* Pick up arguments from (real) locations 0-3 */
	blkcopy(myipaddr, 0, IPLEN);
#else	RARP

	register struct arppak	*apacptr;
	int	pkts;

	blkcopy(wpkt->ep_hdr.e_dest, EBCAST, AR_HLEN);
	wpkt->ep_hdr.e_ptype = hs2net(EP_RARP);
	apacptr = (struct arppak *) wpkt->ep_data;
	apacptr->ar_hrd = hs2net(AR_HRD);
	apacptr->ar_prot = hs2net(AR_PROT);
	apacptr->ar_hlen = AR_HLEN;
	apacptr->ar_plen = AR_PLEN;
	apacptr->ar_op = hs2net(AR_RREQ);
	blkcopy(apacptr->ar_sha, myeaddr, AR_HLEN);
	blkcopy(apacptr->ar_tha, myeaddr, AR_HLEN);
	apacptr = (struct arppak *) rpkt->ep_data;
	for (pkts = 0; pkts < 20; pkts++) {
		if (pkts % 7 == 0)
			ewrite(ewcmd, myeaddr, wpkt, sizeof(struct arppak));
		if (eread(ercmd, rpkt, EMAXPAK) == SYSERR)
			continue;
		if ( net2hs(rpkt->ep_hdr.e_ptype) == EP_RARP &&
		     net2hs(apacptr->ar_op) == AR_RRLY &&
			blkequ(apacptr->ar_tha,myeaddr,AR_HLEN)) {
				blkcopy(myipaddr, apacptr->ar_tpa, IPLEN);
				break; /* got my IP address, can quit */
			}
	}
#endif	RARP
}
