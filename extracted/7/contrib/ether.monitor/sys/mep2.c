/* mep.c - mep */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ctype.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	struct	name	*lookup();
struct	sign	ip_tab[] = {IPS};
struct	sign	arp_tab[] = {ARPS};
extern	Bool	match;
extern	Bool	found;

/*------------------------------------------------------
 * mep - print EP packet information
 *------------------------------------------------------
 */

mep(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, length;
	char	str[80];
	Eaddr	faddr, taddr;
	struct	name	*addptr;

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
				args[i] = addptr->n_epaddr;
				args[i] -= 1;
			case 'f':
				args[i] += 1;
				if(epparse(faddr,args[i]) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				for(j=0; j<6; j++){
					if(faddr[j] != epptr->ep_hdr.e_src[j]){
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
				args[i] = addptr->n_epaddr;
				args[i] -= 1;
			case 't':
				args[i] += 1;
				if(epparse(taddr,args[i]) == SYSERR){
					fprintf(stderr,"Invalid address\n");
					found = FALSE;
					userret();
				}
				for(j=0; j<6; j++){
					if(taddr[j] != epptr->ep_hdr.e_dest[j]){
						userret();
					}
				}
				break;

			case 'l':
				args[i] += 1;
				length = atoi(args[i]);
				if(length > (EMAXPAK-14)){
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
	sprintf(str,"%02x%02x.%02x%02x.%02x%02x -> ",epptr->e_src[0]&0377,epptr->e_src[1]&0377,epptr->e_src[2]&0377,epptr->e_src[3]&0377,epptr->e_src[4]&0377,epptr->e_src[5]&0377);
	sprintf(&str[strlen(str)],"%02x%02x.%02x%02x.%02x%02x\n",epptr->e_dest[0]&0377,epptr->e_dest[1]&0377,epptr->e_dest[2]&0377,epptr->e_dest[3]&0377,epptr->e_dest[4]&0377,epptr->e_dest[5]&0377);
	write(stdout, str, strlen(str));
	switch(net2hs(epptr->e_ptype)) {
		case EP_IP:
			fprintf(stdout,"Packet type: IP protocol\n");
			break;
		case EP_ARP:
			fprintf(stdout,"Packet type: ARP protocol\n");
			break;
		case EP_RARP:
			fprintf(stdout,"Packet type: RARP protocol\n");
			break;
		case EP_PUP:
			fprintf(stdout,"Packet type: Xerox PUP protocal\n");
			break;
		case EP_ECHO:
			fprintf(stdout,"Packet type: Echo\n");
			break;
		case EP_LOOP:
			fprintf(stdout,"Packet type: Looback\n");
			break;
		default:
			fprintf(stdout,"Unknown packet type\n");
			break;
	}
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",epptr->ep_data[i]);
	fprintf(stdout,"\n");

	return(OK);
}
/*------------------------------------------------------------------
 * epparse - parse form like 0800.2b02.0005 into physical address 
 *------------------------------------------------------------------
 */

epparse(addr, fspec)
Eaddr	addr;
char	*fspec;
{
	int	i, j, byte;
	char	ch;

	for(i=0; i<3; i++){
		byte = 0;
		for(j=0; j<2; j++){
			if(isdigit(ch = *fspec++)){
				byte = 16 * byte + (ch - '0');
			}else if(ch=='a' || ch=='b' || ch=='c' || ch=='d'                                        || ch=='e' || ch=='f'){
				byte = 16 * byte + 10 + (ch - 'a');
			}else{
				return(SYSERR);
			}
		}
		if(byte > 256)
			return(SYSERR);
		*addr++ = (char)byte;
		byte = 0;
		for(j=0; j<2; j++){
			if(isdigit(ch = *fspec++)){
				byte = 16 * byte + (ch - '0');
			}else if(ch=='a' || ch=='b' || ch=='c' || ch=='d'                                        || ch=='e' || ch=='f'){
				byte = 16 * byte + 10 + (ch - 'a');
			}else {
				return(SYSERR);
			}
		}
		if(byte > 256 || (i<2 && (ch = *fspec++) != '.'))
			return(SYSERR);
		*addr++ = (char)byte;
	}
	return(OK);
}
