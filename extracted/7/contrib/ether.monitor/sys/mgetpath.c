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
	fprintf(0, "%d.%d.%d.%d\n",faddr[0]&0377,faddr[1]&0377,faddr[2]&0377,faddr[3]&0377);
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
