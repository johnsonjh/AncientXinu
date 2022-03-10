/* mudp.c - mudp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <udp.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
struct	sign	udp_tab[] = {UDPS};

/*------------------------------------------------------
 * mudp - print UDP packet information
 *------------------------------------------------------
 */

mudp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, len,  length;
	char	str[80];
	char	type;
	char	sport[2], dport[2];
	unsigned	portnum;
	struct	ip	*ipptr;
	struct	udp	*udpptr;
	struct	sign	*list;
	struct	sign	*faddptr;
	struct	sign	*taddptr;

	switch(net2hs(epptr->e_ptype)){
		case EP_IP:
			break;
		default:
			userret();
	}
	ipptr = (struct ip *) epptr->ep_data;
	switch(ipptr->i_proto){
		case IPRO_UDP:
			break;
		default:
			userret();
	}
	udpptr = (struct udp *)ipptr->i_data;
	list = udp_tab;
	portnum = net2hs(udpptr->u_sport);
	sport[0] = (char) (portnum & 0xff);
	sport[1] = (char) ((portnum >> 8) & 0xff);
	portnum = net2hs(udpptr->u_dport);
	dport[0] = (char)(portnum & 0xff);
	dport[1] = (char)((portnum>>8) & 0xff);
	type = 5;
	len = IPHLEN - UHLEN;
	if((length=margparse(list,sport,dport,type,len,nargs,args))==SYSERR){
		userret();
	}
	match = TRUE;
	sprintf(str,"%d.%d.%d.%d -> %d.%d.%d.%d\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]&0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	write(stdout, str, strlen(str));
	faddptr = checknum(udp_tab, net2hs(udpptr->u_sport));
	taddptr = checknum(udp_tab, net2hs(udpptr->u_dport));
	sprintf(str,"Sport=%8s(%4u),dport=%8s(%4u),len=%4d,ucksum=%4d\n",faddptr->s_lname, net2hs(udpptr->u_sport),taddptr->s_lname,net2hs(udpptr->u_dport),net2hs(udpptr->u_udplen),udpptr->u_ucksum);
	write(stdout,str,strlen(str));
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",udpptr->u_data[i]&0xff);
	fprintf(stdout,"\n");
	return(OK);
}
