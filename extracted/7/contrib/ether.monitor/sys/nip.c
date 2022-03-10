/* nip.c - nip */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include "../h/ip.h"
#include <udp.h>
#include <icmp.h>
#include "../h/network.h"
#include "../h/monitor.h"
#include <ctype.h>

extern	struct	epacket	*epptr;
extern	struct	name	*lookup();
extern	Bool	match;
extern	Bool	found;

/*------------------------------------------------------
 * nip - print IP packet information
 *------------------------------------------------------
 */

nip(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, length;
	char	str[80];
	IPaddr	faddr, taddr;
	struct	ip	*ipptr;
	struct	udp	*udpptr;
	struct	icmp	*icmpptr;
	struct	name	*addptr;

	switch(net2hs(epptr->e_ptype)){
		case EP_IP:
			break;
		default:
			userret();
	}
	ipptr = (struct ip *) epptr->ep_data;
	length = net2hs(ipptr->i_paclen) - IPHLEN;
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
			sprintf(str,"Type=%2d,code=%2d,cksum=%4d,id=%4d,seq=%4d"                        , icmpptr->ic_typ,icmpptr->ic_code,icmpptr->ic_cksum,                          net2hs(icmpptr->ic_id),net2hs(icmpptr->ic_seq));
			write(stdout,str,strlen(str));
			fprintf(stdout,"\n");
			for(i=0; i<(length-8); i++)
				fprintf(stdout,"%03o ",icmpptr->ic_data[i]&0xff);
			fprintf(stdout,"\n");
			break;
		case IPRO_UDP:
			udpptr = (struct udp *) ipptr->i_data;
			fprintf(stdout,"Display the head of UDP packet:\n");
			sprintf(str,"Sport=%4d,dport=%4d,len=%4d,ucksum=%4d",            			net2hs(udpptr->u_sport),net2hs(udpptr->u_dport),				net2hs(udpptr->u_udplen),udpptr->u_ucksum);
			write(stdout,str,strlen(str));
			fprintf(stdout,"\n");
			for(i=0; i<(length-UHLEN); i++)
				fprintf(stdout,"%03o ",udpptr->u_data[i]&0xff);
			fprintf(stdout,"\n");
			break;
		default:
			fprintf(stdout,"Default\n");
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
