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
	int	i, j, length;
	char	str[80];
	unsigned	*type;
	struct	ip	*ipptr;
	struct	icmp	*icmpptr;
	struct	sign	*addptr;

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
	icmpptr = (struct icmp *)ipptr->i_data;
	length = 8;
	for (i=1; i<nargs; i++){
		switch(args[i][0]){
			case 'F':
				args[i] += 1;
				if(!(addptr=checkname(icmp_tab,args[i]))){
					fprintf(stderr,"Unknown ICMP type\n");
					found = FALSE;
					userret();
				}
				*type = (unsigned)addptr->s_number;
				fprintf(0,"number=%u\n",addptr->s_number);
				fprintf(0,"type=%u\n",*type);
				if(*type != icmpptr->ic_typcod){
					userret();
				}
				break;
			case 'f':
				args[i] += 1;
				if(icmpparse(type,args[i]) == SYSERR){
					fprintf(stderr,"Invalid ICMP type\n");
					found = FALSE;
					userret();
				}
				fprintf(0,"type=%d, typ=%d\n",*type,icmpptr->ic_typcod);
				if(!blkequ(type,&icmpptr->ic_typcod,strlen(type))){
					userret();
				}
				break;

			case 'l':
				args[i] += 1;
				length = atoi(args[i]);
				if(length > (EMAXPAK-IPHLEN-8)){
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
	addptr = checknum(icmp_tab, net2hs(icmpptr->ic_typcod));
	sprintf(str,"typcod=%7s, cksum=%u, id=%u, seq=%u\n",addptr->s_lname,net2hs(icmpptr->ic_cksum),net2hs(icmpptr->ic_id),net2hs(icmpptr->ic_seq));
	write(stdout,str,strlen(str));
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",icmpptr->ic_data[i]&0xff);
	fprintf(stdout,"\n");
	return(OK);
}
icmpparse(addr,fspec)
char	*addr;
char	*fspec;
{
	int	i, byte;
	int	len;
	char	ch;

	byte = 0;
	len = strlen(fspec);
	for(i=0; i<len; i++){
		if(isdigit(ch = *fspec++)){
			byte = 16 * byte + (ch - '0');
		}else if(ch=='a' || ch=='b' || ch=='c' || ch=='d'                                        || ch=='e' || ch=='f'){
			byte = 16 * byte + (ch - 'a');
		}else
			return(SYSERR);
		if(byte > 0x1000)
			return(SYSERR);
	}
	*addr = (char) byte;
	return(OK);
}
