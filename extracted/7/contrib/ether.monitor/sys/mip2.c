/* mip.c - mip */

#include <conf.h>
#include <kernel.h>
#include <ctype.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
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
	struct	name	*addptr;

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
				if(mipparse(faddr,args[i]) == SYSERR){
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
				if(mipparse(taddr,args[i]) == SYSERR){
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
			fprintf(stdout,"Protocl type: ICMP\n");
			break;
		case IPRO_UDP:
			fprintf(stdout,"Protocol type: UDP\n");
			break;
		case IPRO_TCP:
			fprintf(stdout,"Protocol type: TCP\n");
			break;
		default:
			fprintf(stdout,"Other type packet\n");
			break;
	}
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",ipptr->i_data[i]&0xff);
	fprintf(stdout,"\n");

	return(OK);
}
mipparse(addr,fspec)
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
