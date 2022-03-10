/* marp.c - marp */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <arp.h>
#include <ctype.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
char	st[5]={"?", "AR_REQ", "AR_RPLY", "AR_RREQ", "AR_RRLY"};

/*------------------------------------------------------
 * arp - print ARP packet information
 *------------------------------------------------------
 */

marp(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, length;
	char	str[80];
	IPaddr	faddr, taddr;
	struct	arppak	*arpptr;
	struct	name	*addptr;

	length = 28;
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
	arpptr = (struct arppak *) epptr->ep_data;
	sprintf(str,"hrd=%4d, prot=%04x, hlen=%d, plen=%d, op=%7s\n",net2hs(arpptr->ar_hrd),net2hs(arpptr->ar_prot),arpptr->ar_hlen,arpptr->ar_plen,st[net2hs(arpptr->ar_op)]);
	write(stdout, str, strlen(str));
	sprintf(str,"sha=%02x%02x.%02x%02x.%02x%02x,",arpptr->ar_sha[0]&0377,arpptr->ar_sha[1]&0377,arpptr->ar_sha[2]&0377,arpptr->ar_sha[3]&0377,arpptr->ar_sha[4]&0377,arpptr->ar_sha[5]&0377);
	sprintf(&str[strlen(str)]," tha=%02x%02x.%02x%02x.%02x%02x\n",arpptr->ar_tha[0]&0377,arpptr->ar_tha[1]&0377,arpptr->ar_tha[2]&0377,arpptr->ar_tha[3]&0377,arpptr->ar_tha[4]&0377,arpptr->ar_tha[5]&0377);
	write(stdout, str, strlen(str));
	sprintf(str,"spa=%d.%d.%d.%d, tpa=%d.%d.%d.%d\n",arpptr->ar_spa[0]&0377,arpptr->ar_spa[1]&0377,arpptr->ar_spa[2]&0377,arpptr->ar_spa[3]&0377,arpptr->ar_tpa[0]&0377,arpptr->ar_tpa[1]&0377,arpptr->ar_tpa[2]&0377,arpptr->ar_tpa[3]&0377);
	write(stdout, str, strlen(str));

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
