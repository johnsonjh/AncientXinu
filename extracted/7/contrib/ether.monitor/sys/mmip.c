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
extern	struct	name	*lookaddr();
extern	Bool	match;
extern	Bool	found;
struct	sign	ip_tab[] = {IPS};

/*------------------------------------------------------
 * mip - print IP packet information
 *------------------------------------------------------
 */

mip(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, len, length;
	char	str[80];
	char	type;
	IPaddr	saddr[4], daddr[4];
	struct	ip	*ipptr;
	struct	name	*sptr;
	struct	name	*dptr;
	struct	sign	*list;
	struct	sign	*addptr;

	switch(net2hs(epptr->e_ptype)){
		case EP_IP:
			break;
		default:
			userret();
	}
	ipptr = (struct ip *) epptr->ep_data;
	blkcopy(saddr, ipptr->i_src, IPLEN);
	blkcopy(daddr, ipptr->i_dest, IPLEN);
	type = 2;
	len = IPHLEN ;
	if((length=margparse(list,saddr,daddr,type,len,nargs,args))==SYSERR){
		userret();
	}
	match = TRUE;
	sprintf(str,"%d.%d.%d.%d",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]&0377,ipptr->i_src[3]&0377);
	sptr = lookaddr(table, &str[0]);
	sprintf(str,"%d.%d.%d.%d",ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	dptr = lookaddr(table, &str[0]);
	fprintf(stdout,"%9s  ->   %9s\n",sptr->n_nam,dptr->n_nam);
	sprintf(str,"%02x%02x.%02x%02x.%02x%02x -> ",epptr->e_src[0]&0377,epptr->e_src[1]&0377,epptr->e_src[2]&0377,epptr->e_src[3]&0377,epptr->e_src[4]&0377,epptr->e_src[5]&0377);
	sprintf(&str[strlen(str)],"%02x%02x.%02x%02x.%02x%02x\n",epptr->e_dest[0]&0377,epptr->e_dest[1]&0377,epptr->e_dest[2]&0377,epptr->e_dest[3]&0377,epptr->e_dest[4]&0377,epptr->e_dest[5]&0377);
	write(stdout, str, strlen(str));
	sprintf(str,"%u.%u.%u.%u -> %u.%u.%u.%u\n",ipptr->i_src[0]&0377,ipptr->i_src[1]&0377,ipptr->i_src[2]%0377,ipptr->i_src[3]&0377,ipptr->i_dest[0]&0377,ipptr->i_dest[1]&0377,ipptr->i_dest[2]&0377,ipptr->i_dest[3]&0377);
	write(stdout,str,strlen(str));
	addptr = checknum(ip_tab, (unsigned)ipptr->i_proto);
	fprintf(stdout,"Packet type: %8s\n", addptr->s_lname);
	for(i=0; i<length; i++)
		fprintf(stdout,"%03o ",ipptr->i_data[i]&0xff);
	fprintf(stdout,"\n");

	return(OK);
}
