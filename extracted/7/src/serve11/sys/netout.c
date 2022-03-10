/* netout.c - netout */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  netout  -  start network by finding address and forward IP packets
 *------------------------------------------------------------------------
 */
PROCESS	netout(userpid, icmpp)
int	userpid;
int	icmpp;
{
	struct	epacket	*packet;
	struct	ip	*ipptr;
	long	tim;
	int	len;
	IPaddr	addr;

	getaddr(addr);
	gettime(&tim);
	resume(userpid);
	while (TRUE) {
		packet = (struct epacket *) preceive(icmpp);
		ipptr = (struct ip *) packet->ep_data;
		blkcopy (addr, ipptr->i_dest, IPLEN);
		len = net2hs(ipptr->i_paclen) - IPHLEN;
		ipsend(addr, packet, len);
	}
}
