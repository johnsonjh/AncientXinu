/* mtabinit.c - mtabinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <arp.h>
#include <net.h>
#include "../h/network.h"
#include "../h/monitor.h"

mtabinit(list)
struct	name	*list;
{
	int	arindex;
	char	str[20];
	IPaddr	taddr;
	struct	arpent	*atabptr;

	fprintf(0,"Welcome to run the network monitor !\n");
	fprintf(0,"Please wait a mniute .\n");
	while(!blkequ(list->n_ipaddr, "128.10.2.31", strlen(list->n_ipaddr))){
		ipparse(taddr, list->n_ipaddr);
		if((arindex = mgetpath(taddr)) == SYSERR){
			blkcopy(list->n_epaddr, EAZERO, EPADLEN);
			fprintf(0, "%s: No response to ARP\n", list->n_nam);
		}else{
			atabptr = &Arp.arptab[arindex];
			sprintf(str,"%02x%02x.%02x%02x.%02x%02x",atabptr->arp_Ead[0]&0377,atabptr->arp_Ead[1]&0377,atabptr->arp_Ead[2]&0377,atabptr->arp_Ead[3]&0377,atabptr->arp_Ead[4]&0377,atabptr->arp_Ead[5]&0377);
			blkcopy(list->n_epaddr, str, strlen(str));
		}
		list++;
	}
}

/* mgetpath.c - mgetpath */


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
		faddr = &Net.gateway;
	result = getpath(faddr);
	arpptr = &Arp.arptab[ result ];
	if (arpptr->arp_state != AR_RSLVD) {
		arpptr->arp_state = AR_RGATE;
		result = getpath(&Net.gateway);
		arpptr = &Arp.arptab[result];
		if (arpptr->arp_state != AR_RSLVD) {
			signal(Net.nmutex);
			return(SYSERR);
		}
	}
	signal(Net.nmutex);
	return(result);
}
