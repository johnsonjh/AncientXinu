/* mgetpath.c - mgetpath */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  mgetpath  -  get a physical network address for a given IP address
 *------------------------------------------------------------------------
 */
mgetpath(faddr)
IPaddr	faddr;
{
	int	result;
	struct	arpent	*arpptr;
	IPaddr	mynet, destnet;


	getnet(mynet);
	netnum(destnet, faddr);
	wait(Net.nmutex);
	if (!blkequ(destnet, mynet, IPLEN))
		faddr = Net.gateway;
	result = getpath(faddr);
	arpptr = &Arp.arptab[ result ];
	if (arpptr->arp_state != AR_RSLVD) {
		arpptr->arp_state = AR_RGATE;
		result = getpath(Net.gateway);
		arpptr = &Arp.arptab[result];
		if (arpptr->arp_state != AR_RSLVD) {
			signal(Net.nmutex);
			return(SYSERR);
		}
	}
	signal(Net.nmutex);
	return(result);
}
