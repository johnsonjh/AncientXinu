/* mip.c - mip */

#include <conf.h>
#include <kernel.h>
#include <ctype.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <udp.h>
#include <icmp.h>
#include "../h/tcp.h"
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	struct	name	*lookup();
extern	Bool	match;
extern	Bool	found;

/*------------------------------------------------------
 * mip - print IP packet information
 *------------------------------------------------------
 */

mip(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, length;
	char	str[80];
	IPaddr	faddr, taddr;
	struct	ip	*ipptr;
	struct	udp	*udpptr;
	struct	icmp	*icmpptr;
	struct	tcp	*tcpptr;
	struct	name	*addptr;
	struct	sign	*faddptr;
	struct	sign	*taddptr;

	switch(net2hs(epptr->e_ptype)){
		case EP_IP:
			break;
		default:
			userret();
	}
	ipptr = (struct ip *) epptr->ep_data;
	length = 8;
	for (i=1; i<nargs; i++){
		switch(args[i][0]){
			case 'F':
				args[i] += 1;
				if((addptr=lookup(table,args[i])) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				args[i] = addptr->n_ipaddr;
				write(stdout,str,strlen(str));
				args[i] -= 1;
			case 'f':
				args[i] += 1;
				if(ipparse(faddr,args[i]) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				for(j=0; j<4; j++){
					if(faddr[j] != ipptr->i_src[j]){
						userret();
					}
				}
				break;

			case 'T':
				args[i] += 1;
				if((addptr=lookup(table,args[i])) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				args[i] = addptr->n_ipaddr;
				args[i] -= 1;
			case 't':
				args[i] += 1;
				if(ipparse(taddr,args[i]) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				for(j=0; j<4; j++){
					if(taddr[j] != ipptr->i_dest[j]){
						userret();
					}
				}
				break;

			case 'l':
				args[i] += 1;
				length = atoi(args[i]);
				if(length > (EMAXPAK-IPHLEN)){
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
	sprintf(str,"%d.%d.%d.%d -> %d.%d.%d.%d\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]%0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	write(stdout,str,strlen(str));
	switch(ipptr->i_proto){
		case IPRO_ICMP:
			icmpptr = (struct icmp *) ipptr->i_data;
			fprintf(stdout,"Display the head of ICMP packet:\n");
			faddptr = checknum(icmp_tab, icmpptr->ic_typ);
			sprintf(str,"Type=%7s,code=%2d,cksum=%4d,id=%4d,seq=%4d"                        , faddptr->s_lname,icmpptr->ic_code,icmpptr->ic_cksum,                          net2hs(icmpptr->ic_id),net2hs(icmpptr->ic_seq));
			write(stdout,str,strlen(str));
			fprintf(stdout,"\n");
			for(i=0; i<length; i++)
				fprintf(stdout,"%03o ",icmpptr->ic_data[i]&0xff);
			fprintf(stdout,"\n");
			break;
		case IPRO_UDP:
			udpptr = (struct udp *) ipptr->i_data;
			fprintf(stdout,"Display the head of UDP packet:\n");
			faddptr = checknum(udp_tab, net2hs(udpptr->u_sport));
			taddptr = checknum(udp_tab, net2hs(udpptr->u_dport));
			sprintf(str,"Sport=%8s(%4u),dport=%8s(%4u),len=%4d,ucksum=%4d",faddptr->s_lname, net2hs(udpptr->u_sport),taddptr->s_name,net2hs(udpptr->u_dport),net2hs(udpptr->u_udplen),udpptr->u_ucksum);
			write(stdout,str,strlen(str));
			fprintf(stdout,"\n");
			for(i=0; i<length; i++)
				fprintf(stdout,"%03o ",udpptr->u_data[i]&0xff);
			fprintf(stdout,"\n");
			break;
		case IPRO_TCP:
			tcpptr = (struct tcp *) ipptr->i_data;
			fprintf(stdout,"Display the head of TCP packet:\n");
			faddptr = checknum(tcp_tab, net2hs(tcpptr->tc_sport));
			taddptr = checknum(tcp_tab, net2hs(tcpptr->tc_dport));
			sprintf(str,"%9s(%4u) -> %9s(%4u)\n",faddptr->s_lname,net2hs(tcpptr->tc_sport),taddptr->s_lname,net2hs(tcpptr->tc_dport));
			write(stdout,str,strlen(str));
			sprintf(str,"seq=%ul ack=%ul off=%o flag=%o win=%u sum=%u urp=%u\n",net2hl(tcpptr->tc_seq),net2hl(tcpptr->tc_ack),tcpptr->tc_off,tcpptr->tc_flags,net2hs(tcpptr->tc_win),net2hs(tcpptr->tc_sum),net2hs(tcpptr->tc_urp));
			write(stdout, str, strlen(str));
			for(i=0; i<length; i++)
				fprintf(stdout,"%03o ",tcpptr->tc_data[i]&0xff);
			fprintf(stdout,"\n");
			break;
		default:
			fprintf(stdout,"Other type packet\n");
			break;
	}

	return(OK);
}
ipparse(addr,fspec)
IPaddr	addr;
char	*fspec;
{
	int	i, byte;
	char	ch;

	for(i=0; i<4; i++){
		byte = 0;
		while(isdigit(ch = *fspec++))
			byte = 10 * byte + (ch - '0');
		if(byte > 256 || (i < 3 && ch != '.'))
			return(SYSERR);
		*addr++ = (char) byte;
	}
	return(OK);
}
