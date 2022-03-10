/* marp.c - marp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <arp.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
struct	sign	arp_tab[] = {ARPS};

/*------------------------------------------------------
 * marp - print ARP packet information
 *------------------------------------------------------
 */

marp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, len, length;
	char	str[80];
	char	type;
	IPaddr	saddr, daddr;
	struct	arppak	*arpptr;
	struct	sign	*addptr;
	struct	sign	*list;

	fprintf(0,"ptype=%04x\n",net2hs(epptr->e_ptype));
	switch(net2hs(epptr->e_ptype)){
		case EP_ARP:
			break;
		default:
			userret();
	}
	arpptr = (struct arppak *)epptr->ep_data;
	blkcopy(saddr, arpptr->ar_spa, IPLEN);
	blkcopy(daddr, arpptr->ar_tpa, IPLEN);
	type = 3;
	len = EMAXPAK - 28 ;
	if((length=margparse(list,saddr,daddr,type,len,nargs,args))==SYSERR){
		userret();
	}
	match = TRUE;
	addptr = checknum(arp_tab, net2hs(arpptr->ar_op));
	sprintf(str,"hrd=%4d, prot=%04x, hlen=%d, plen=%d, op=%7s\n",net2hs(arpptr->ar_hrd),net2hs(arpptr->ar_prot),arpptr->ar_hlen,arpptr->ar_plen,addptr->s_lname);
	write(stdout, str, strlen(str));
	sprintf(str,"sha=%02x%02x.%02x%02x.%02x%02x,",arpptr->ar_sha[0]&0377,arpptr->ar_sha[1]&0377,arpptr->ar_sha[2]&0377,arpptr->ar_sha[3]&0377,arpptr->ar_sha[4]&0377,arpptr->ar_sha[5]&0377);
	sprintf(&str[strlen(str)]," tha=%02x%02x.%02x%02x.%02x%02x\n",arpptr->ar_tha[0]&0377,arpptr->ar_tha[1]&0377,arpptr->ar_tha[2]&0377,arpptr->ar_tha[3]&0377,arpptr->ar_tha[4]&0377,arpptr->ar_tha[5]&0377);
	write(stdout, str, strlen(str));
	sprintf(str,"spa=%d.%d.%d.%d, tpa=%d.%d.%d.%d\n",arpptr->ar_spa[0]&0377,arpptr->ar_spa[1]&0377,arpptr->ar_spa[2]&0377,arpptr->ar_spa[3]&0377,arpptr->ar_tpa[0]&0377,arpptr->ar_tpa[1]&0377,arpptr->ar_tpa[2]&0377,arpptr->ar_tpa[3]&0377);
	write(stdout, str, strlen(str));

	return(OK);
}
