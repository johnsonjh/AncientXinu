/* mep.c - mep */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <arp.h>
#include <ctype.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	struct	name	*lookaddr();
extern	Bool	match;
extern	Bool	found;
struct	sign	ep_tab[] = {EPS};

/*------------------------------------------------------
 * mep - print EP packet information
 *------------------------------------------------------
 */

mep(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, len, length;
	char	str[80];
	char	type;
	Eaddr	saddr, daddr;
	struct	name	*sptr;
	struct	name	*dptr;
	struct	sign	*list;
	struct	sign	*addptr;
	struct	ip	*ipptr;
	struct	arppak	*arpptr;

	blkcopy(saddr, epptr->ep_hdr.e_src, EPADLEN);
	blkcopy(daddr, epptr->ep_hdr.e_dest, EPADLEN);
	type = 1;
	len = 14;
	if((length=margparse(list, saddr, daddr, type, len, nargs, args)) == SYSERR){
		userret();
	}
	match = TRUE;
	if(net2hs(epptr->e_ptype)==EP_ARP || net2hs(epptr->e_ptype)==EP_RARP){
		arpptr = (struct arppak*)epptr->ep_data;
		sprintf(str,"%d.%d.%d.%d",arpptr->ar_spa[0]&0xff,arpptr->ar_spa[1]&0xff,arpptr->ar_spa[2]&0xff,arpptr->ar_spa[3]&0xff);
		sptr = lookaddr(table, &str[0]);
		sprintf(str,"%d.%d.%d.%d",arpptr->ar_tpa[0]&0xff,arpptr->ar_tpa[1]&0xff,arpptr->ar_tpa[2]&0xff,arpptr->ar_tpa[3]&0xff);
		dptr = lookaddr(table, &str[0]);
	}else if(net2hs(epptr->e_ptype)==EP_IP){
		ipptr = (struct ip*)epptr->ep_data;
		sprintf(str,"%d.%d.%d.%d",ipptr->i_src[0]&0xff,ipptr->i_src[1]&0xff,ipptr->i_src[2]&0xff,ipptr->i_src[3]&0xff);
		sptr = lookaddr(table, &str[0]);
		sprintf(str,"%d.%d.%d.%d",ipptr->i_dest[0]&0xff,ipptr->i_dest[1]&0xff,ipptr->i_dest[2]&0xff,ipptr->i_dest[3]&0xff);
		dptr = lookaddr(table, &str[0]);
	}else {
		sptr = " ";
		dptr = " ";
	}
	fprintf(stdout,"%9s   ->   %9s\n",sptr->n_nam,dptr->n_nam);
	sprintf(str,"%02x%02x.%02x%02x.%02x%02x -> ",epptr->e_src[0]&0377,epptr->e_src[1]&0377,epptr->e_src[2]&0377,epptr->e_src[3]&0377,epptr->e_src[4]&0377,epptr->e_src[5]&0377);
	sprintf(&str[strlen(str)],"%02x%02x.%02x%02x.%02x%02x\n",epptr->e_dest[0]&0377,epptr->e_dest[1]&0377,epptr->e_dest[2]&0377,epptr->e_dest[3]&0377,epptr->e_dest[4]&0377,epptr->e_dest[5]&0377);
	write(stdout, str, strlen(str));
	addptr = checknum(ep_tab, net2hs(epptr->e_ptype));
	fprintf(stdout,"Packet type: %8s\n", addptr->s_lname);
	for(i=0; i<length; i++){
		fprintf(stdout,"%03o ",epptr->ep_data[i]&0xff);
	}
	fprintf(stdout,"\n");
	return(OK);
}
