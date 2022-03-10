/* mep.c - mep */

#include <conf.h>
#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <ctype.h>
#include "../h/network.h"
#include "../h/monitor.h"

extern	struct	epacket	*epptr;
extern	Bool	match;
extern	Bool	found;
struct	sign	ep_tab[] = {EPS};

/*------------------------------------------------------
 * mep - print EP packet information
 *------------------------------------------------------
 */

mep(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j, len, length;
	char	str[80];
	char	type;
	Eaddr	saddr, daddr;
	struct	sign	*list;
	struct	sign	*addptr;

	blkcopy(saddr, epptr->ep_hdr.e_src, EPADLEN);
	blkcopy(daddr, epptr->ep_hdr.e_dest, EPADLEN);
	type = 1;
	len = 14;
	if((length=margparse(list, saddr, daddr, type, len, nargs, args)) == SYSERR){
		userret();
	}
	match = TRUE;
	sprintf(str,"%02x%02x.%02x%02x.%02x%02x -> ",epptr->e_src[0]&0377,epptr->e_src[1]&0377,epptr->e_src[2]&0377,epptr->e_src[3]&0377,epptr->e_src[4]&0377,epptr->e_src[5]&0377);
	sprintf(&str[strlen(str)],"%02x%02x.%02x%02x.%02x%02x\n",epptr->e_dest[0]&0377,epptr->e_dest[1]&0377,epptr->e_dest[2]&0377,epptr->e_dest[3]&0377,epptr->e_dest[4]&0377,epptr->e_dest[5]&0377);
	write(stdout, str, strlen(str));
	addptr = checknum(ep_tab, net2hs(epptr->e_ptype));
	fprintf(stdout,"Packet type: %8s\n", addptr->s_lname);
	for(i=0; i<length; i++){
		fprintf(stdout,"%03o ",epptr->ep_data[i]&0xff);
	}
	fprintf(stdout,"\n");
	return(OK);
}
