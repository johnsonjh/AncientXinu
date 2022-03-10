/* mudp.c - mudp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include "../h/ip.h"
#include <udp.h>
#include "../h/network.h"
#include "../h/monitor.h"
#include <ctype.h>

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
	int	i, j, length;
	char	str[80];
	unsigned	*fport, *tport;
	struct	ip	*ipptr;
	struct	udp	*udpptr;
	struct	sign	*addptr;
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
	length = 8;
	for (i=1; i<nargs; i++){
		switch(args[i][0]){
			case 'F':
				args[i] += 1;
				if(!(addptr=checkname(udp_tab,args[i]))){
					fprintf(stderr,"Unknown port name\n");
					found = FALSE;
					userret();
				}
				*fport = addptr->s_number;
				fprintf(0,"number=%d\n",addptr->s_number);
				fprintf(0,"fport=%u\n",*fport);
				if(*fport != net2hs(udpptr->u_sport)){
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
				fprintf(0,"fport=%u, sport=%u\n",*fport,net2hs(udpptr->u_sport));
				if(*fport != net2hs(udpptr->u_sport)){
					userret();
				}
				break;

			case 'T':
				args[i] += 1;
				if(!(addptr=checkname(udp_tab,args[i]))){
					fprintf(stderr,"Unknown port number\n");
					found = FALSE;
					userret();
				}
				*tport = addptr->s_number;
				if(*tport != net2hs(udpptr->u_dport)){
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
				if(*tport != net2hs(udpptr->u_dport)){
					userret();
				}
				break;

			case 'l':
				args[i] += 1;
				length = atoi(args[i]);
				if(length > (EMAXPAK-IPHLEN-UHLEN)){
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
	faddptr = checknum(udp_tab, net2hs(udpptr->u_sport));
	taddptr = checknum(udp_tab, net2hs(udpptr->u_dport));
	sprintf(str,"Sport=%8s(%4u),dport=%8s(%4u),len=%4d,ucksum=%4d",faddptr->s_lname, net2hs(udpptr->u_sport),taddptr->s_lname,net2hs(udpptr->u_dport),net2hs(udpptr->u_udplen),udpptr->u_ucksum);
	write(stdout,str,strlen(str));
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",udpptr->u_data[i]&0xff);
	fprintf(stdout,"\n");
	return(OK);
}
udpparse(addr,fspec)
unsigned	*addr;
char	*fspec;
{
	int	i;
	int	len;
	char	ch;
	unsigned	port;

	port = 0;
	len = strlen(fspec);
	fprintf(0,"Len=%d\n",len);
	if(len > 4)
		return(SYSERR);
	for(i=0; i<len; i++){
		if(isdigit(ch = *fspec++)){
			port = (unsigned)(10*port + (ch - '0'));
		}else{
			return(SYSERR);
		}
	}
	fprintf(0,"port=%u\n",port);
	*addr = port;
	return(OK);
}
