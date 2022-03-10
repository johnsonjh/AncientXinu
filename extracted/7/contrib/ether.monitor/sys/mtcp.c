/* mtcp.c - mtcp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include "../h/tcp.h"
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
struct	sign	tcp_tab[] = {TCPS};

#define	TCPHLEN		20

/*------------------------------------------------------
 * mtcp - print TCP packet information
 *------------------------------------------------------
 */

mtcp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, len, length;
	char	str[80];
	char	type;
	char	sport[2], dport[2];
	unsigned	portnum;
	struct	ip	*ipptr;
	struct	tcp	*tcpptr;
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
		case IPRO_TCP:
			break;
		default:
			userret();
	}
	tcpptr = (struct tcp *)ipptr->i_data;
	list = tcp_tab;
	portnum = net2hs(tcpptr->tc_sport);
	sport[0] = (char) (portnum & 0xff);
	sport[1] = (char) ((portnum >> 8) & 0xff);
	portnum = net2hs(tcpptr->tc_dport);
	dport[0] = (char)(portnum & 0xff);
	dport[1] = (char)((portnum>>8) & 0xff);
	type = 7;
	len = IPHLEN - TCPHLEN;
	if((length=margparse(list,sport,dport,type,len,nargs,args))==SYSERR){
		userret();
	}
	match = TRUE;
	sprintf(str,"%d.%d.%d.%d -> %d.%d.%d.%d\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]&0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	write(stdout, str, strlen(str));
	faddptr = checknum(tcp_tab, net2hs(tcpptr->tc_sport));
	taddptr = checknum(tcp_tab, net2hs(tcpptr->tc_dport));
	sprintf(str,"%9s(%4u) -> %9s(%4u)\n",faddptr->s_lname,net2hs(tcpptr->tc_sport),taddptr->s_lname,net2hs(tcpptr->tc_dport));
	write(stdout,str,strlen(str));
	sprintf(str,"seq=%ul ack=%ul off=%o flag=%o win=%u sum=%u urp=%u\n",net2hl(tcpptr->tc_seq),net2hl(tcpptr->tc_ack),tcpptr->tc_off,tcpptr->tc_flags,net2hs(tcpptr->tc_win),net2hs(tcpptr->tc_sum),net2hs(tcpptr->tc_urp));
	write(stdout, str, strlen(str));
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",tcpptr->tc_data[i]&0xff);
	fprintf(stdout,"\n");
	return(OK);
}
