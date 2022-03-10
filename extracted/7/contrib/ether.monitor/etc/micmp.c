/* micmp.c - micmp */

#include <conf.h>
#include <kernel.h>
#include <ctype.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include "../h/icmp.h"
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
struct	sign	icmp_tab[] = {ICMPS};

/*------------------------------------------------------
 * micmp - print ICMP packet information
 *------------------------------------------------------
 */

micmp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, len, length;
	char	str[80];
	char	type;
	char	sport[2], dport[2];
	unsigned	portnum;
	struct	ip	*ipptr;
	struct	icmp	*icmpptr;
	struct	sign	*list;
	struct	sign	*addptr;

	switch(net2hs(epptr->e_ptype)){
		case EP_IP:
			break;
		default:
			userret();
	}
	switch(ipptr->i_proto){
		case IPRO_ICMP:
			break;
		default:
			userret();
	}
	ipptr = (struct ip *) epptr->ep_data;
	icmpptr = (struct icmp *)ipptr->i_data;
	list = icmp_tab;
	portnum = net2hs(icmpptr->ic_typcod);
	sport[0] = (char) (portnum & 0xff);
	sport[1] = (char) ((portnum >> 8) & 0xff);
	portnum = net2hs(icmpptr->ic_typcod);
	dport[0] = (char)(portnum & 0xff);
	dport[1] = (char)((portnum>>8) & 0xff);
	type = 6;
	len = IPHLEN - 8;
	if((length=margparse(list,sport,dport,type,len,nargs,args))==SYSERR){
		userret();
	}
	match = TRUE;
	sprintf(str,"%d.%d.%d.%d -> %d.%d.%d.%d\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]&0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	write(stdout, str, strlen(str));
	addptr = checknum(icmp_tab, net2hs(icmpptr->ic_typcod));
	sprintf(str,"typcod=%7s, cksum=%u, id=%u, seq=%u\n",addptr->s_lname,net2hs(icmpptr->ic_cksum),net2hs(icmpptr->ic_id),net2hs(icmpptr->ic_seq));
	write(stdout,str,strlen(str));
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",icmpptr->ic_data[i]&0xff);
	fprintf(stdout,"\n");
	return(OK);
}
