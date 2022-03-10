/* mtcp.c - mtcp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <ctype.h>
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
	int	i, j, length;
	char	str[80];
	unsigned	*fport, *tport;
	struct	ip	*ipptr;
	struct	tcp	*tcpptr;
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
	length = 8;
	for (i=1; i<nargs; i++){
		switch(args[i][0]){
			case 'F':
				args[i] += 1;
				if(!(faddptr=checkname(tcp_tab,args[i]))){
					fprintf(stderr,"Unknown port name\n");
					found = FALSE;
					userret();
				}
				*fport = faddptr->s_number;
				fprintf(0,"number=%d\n",faddptr->s_number);
				fprintf(0,"fport=%u\n",*fport);
				if(*fport != net2hs(tcpptr->tc_sport)){
					userret();
				}
				break;
			case 'f':
				args[i] += 1;
				if(udpparse(fport,args[i]) == SYSERR){
					fprintf(stderr,"Invalid port number\n");
					found = FALSE;
					userret();
				}
				fprintf(0,"fport=%u, sport=%u\n",*fport,net2hs(tcpptr->tc_sport));
				if(*fport != net2hs(tcpptr->tc_sport)){
					userret();
				}
				break;

			case 'T':
				args[i] += 1;
				if(!(taddptr=checkname(tcp_tab,args[i]))){
					fprintf(stderr,"Unknown port number\n");
					found = FALSE;
					userret();
				}
				*tport = taddptr->s_number;
				if(*tport != net2hs(tcpptr->tc_dport)){
					userret();
				}
				break;
			case 't':
				args[i] += 1;
				if(udpparse(tport,args[i]) == SYSERR){
					fprintf(stderr,"Invalid port number\n");
					found = FALSE;
					userret();
				}
				fprintf(0,"port=%u\n",*tport);
				if(*tport != net2hs(tcpptr->tc_dport)){
					userret();
				}
				break;

			case 'l':
				args[i] += 1;
				length = atoi(args[i]);
				if(length > (EMAXPAK-IPHLEN-TCPHLEN)){
					fprintf(stderr,"Invalid data length\n");
					found = FALSE;
					userret();
				}
				break;

			default:
				fprintf(stderr,"Syntax error\n");
				found = FALSE;
				userret();
		}
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
