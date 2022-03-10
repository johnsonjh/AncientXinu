/* margparse.c - margparse */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <deqna.h>
#include <ether.h>
#include <ip.h>
#include <ctype.h>
#include "../h/monitor.h

#define	EP	1
#define IP	2
#define	ARP	3
#define	RARP	4
#define	UDP	5
#define	ICMP	6
#define	TCP	7

extern struct	name	*lookup();
extern struct	sign	*checkname();
extern	Bool	found;
extern	int	or;

/*-------------------------------------------------------------------
 * margparse - parse auguments in a command
 *-------------------------------------------------------------------
 */

margparse(list, saddr, daddr, type, len, num, arg)
char	*list, saddr[], daddr[], type, *arg[];
int	len, num;
{
	int	i, count, stderr, length, result;
	char	*addptr, *add, faddr[6], taddr[6], port[2];

	stderr = 0;
	length = 28;
	for(i=1; i<num; i++){
	   switch(arg[i][0]){
		case 'D':
			arg[i] += 1;
			switch(type){
	   		case EP:
				if((addptr=lookup(table,arg[i])) == SYSERR){
					fprintf(stderr,"Unknown machine name\n");
	   				found = TRUE;
	   				return(SYSERR);
	   			}
	   			arg[i] = addptr->n_epaddr;
	   			break;
	   		case IP:
	   		case ARP:
	   		case RARP:
				if((addptr=lookup(table,arg[i])) == SYSERR){
					fprintf(stderr,"Unknown machine name\n");
	   				found = TRUE;
	   				return(SYSERR);
	   			}
	   			arg[i] = addptr->n_ipaddr;
	   			break;
	   		case UDP:
	   		case ICMP:
	   		case TCP:
				if(!(addptr=checkname(list,arg[i]))){
					fprintf(stderr,"Unknown port name\n");
					found = TRUE;
					return(SYSERR);
				}
				port[0] = (char) (addptr->s_number)&0xff;
				port[1] = (char) ((addptr->s_number)>>8)&0xff;
				if(!blkequ(&port[0],daddr,2)){
					if(or == 0){
						found = FALSE;
						return(SYSERR);
					}else {
						or--;
						break;
					}
				}
				found = TRUE;
				break;
			}
			if(type==UDP || type==ICMP || type==TCP){
				break;
			}else
			arg[i] -= 1;
		case 'd':
			arg[i] += 1;
			add = &taddr[0];
			switch(type){
			case EP:
				result = epparse(add,arg[i]);
				count = 6;
				break;
			case IP:
			case ARP:
			case RARP:
				result = ipparse(add,arg[i]);
				count = 4;
				break;
			case UDP:
			case TCP:
				result = udpparse(add,arg[i]);
				count = 2;
				break;
			case ICMP:
				result = icmpparse(add,arg[i]);
				count = 2;
				break;
			}
			if(result == SYSERR){
				fprintf(stderr,"Invalid address\n");
				found = TRUE;
				return(SYSERR);
			}
			if(!blkequ(taddr, daddr, count)){
				if(or == 0){
					found = FALSE;
					return(SYSERR);
				}else {
					or--;
					break;
				}
			}
			found = TRUE;
			break;
		case 'S':
			arg[i] += 1;
			switch(type){
	   		case EP:
				if((addptr=lookup(table,arg[i])) == SYSERR){
					fprintf(stderr,"Unknown machine name\n");
	   				found = TRUE;
	   				return(SYSERR);
	   			}
	   			arg[i] = addptr->n_epaddr;
	   			break;
	   		case IP:
	   		case ARP:
	   		case RARP:
				if((addptr=lookup(table,arg[i])) == SYSERR){
					fprintf(stderr,"Unknown machine name\n");
	   				found = TRUE;
	   				return(SYSERR);
	   			}
	   			arg[i] = addptr->n_ipaddr;
	   			break;
	   		case UDP:
	   		case ICMP:
	   		case TCP:
				if(!(addptr=checkname(list,arg[i]))){
					fprintf(stderr,"Unknown port name\n");
					found = TRUE;
					return(SYSERR);
				}
				port[0] = (char) (addptr->s_number)&0xff;
				port[1] = (char) ((addptr->s_number)>>8)&0xff;
				if(!blkequ(&port[0],saddr,2)){
					if(or == 0){
						found = FALSE;
						return(SYSERR);
					}else {
						or--;
						break;
					}
				}
				found = TRUE;
				break;
			}
			if(type==UDP || type==ICMP || type==TCP){
				break;
			}else
			arg[i] -= 1;
		case 's':
			arg[i] += 1;
			add = &faddr[0];
			switch(type){
			case EP:
				result = epparse(add,arg[i]);
				count = 6;
				break;
			case IP:
			case ARP:
			case RARP:
				result = ipparse(add,arg[i]);
				count = 4;
				break;
			case UDP:
			case TCP:
				result = udpparse(add,arg[i]);
				count = 2;
				break;
			case ICMP:
				result = icmpparse(add,arg[i]);
				count = 2;
				break;
			}
			if(result == SYSERR){
				fprintf(stderr,"Invalid address\n");
				found = TRUE;
				return(SYSERR);
			}
			if(!(blkequ(faddr, saddr, count))){
				if(or == 0){
					found = FALSE;
					return(SYSERR);
				}else {
					or--;
					break;
				}
			}
			found = TRUE;
			break;
		case 'l':
			arg[i] += 1;
			length = atoi(arg[i]);
			if(length > (EMAXPAK - len)){
				fprintf(stderr,"Invalid data length\n");
				found = TRUE;
				return(SYSERR);
			}
			break;
		default:
			fprintf(stderr,"Syntax error\n");
			found = TRUE;
			return(SYSERR);
	    }
	}
	return(length);
}
/*----------------------------------------------------------------
 * epparse - parse form like 0800.2b02.0005 into physical address
 *----------------------------------------------------------------
 */

epparse(addr, fspec)
Eaddr	addr;
char	*fspec;
{
	int	i, j, byte;
	char	ch;
	char	*addrptr;

	addrptr = &addr[0];
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
		*addrptr++ = (char)byte;
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
		*addrptr++ = (char)byte;
	}
	return(OK);
}

/*------------------------------------------------------------------
 * ipparse - parse form like 128.10.2.8 into internet address(IP)
 *------------------------------------------------------------------
 */

ipparse(addr,fspec)
IPaddr	addr;
char	*fspec;
{
	int	i;
	unsigned	byte;
	char	ch;
	char	*addptr;

	addptr = &addr[0];
	for(i=0; i<4; i++){
		byte = 0;
		while(isdigit(ch = *fspec++))
			byte = 10 * byte + (ch - '0');
		if(byte > 256 || (i < 3 && ch != '.'))
			return(SYSERR);
		*addptr++ = (char) byte;
	}
	return(OK);
}

/*---------------------------------------------------------------------------
 * udpparse - parse ascii code like 1023 into two charactors like 3 and 255
 *---------------------------------------------------------------------------
 */

udpparse(addr,fspec)
char	addr[2];
char	*fspec;
{
	int	i;
	int	len;
	char	ch;
	char	*addptr;
	unsigned	port;

	port = 0;
	len = strlen(fspec);
	if(len > 4)
		return(SYSERR);
	addptr = &addr[0];
	for(i=0; i<len; i++){
		if(isdigit(ch = *fspec++)){
			port = (unsigned)(10*port + (ch - '0'));
		}else{
			return(SYSERR);
		}
	}
	*addptr++ = (char) port&0xff;
	*addptr = (char) (port>>8)&0xff;
	return(OK);
}

/*---------------------------------------------------------------------------
 * icmparse-parse a hexadecimal digit like 503 into two charactors like 5 and 3
 *___________________________________________________________________________
 */

icmpparse(addr,fspec)
char	addr[2];
char	*fspec;
{
	int	i, byte;
	int	len;
	char	ch;
	char	*addptr;

	byte = 0;
	addptr = &addr[0];
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
	*addptr++ = (char) byte&0xff;
	*addptr = (char) (byte>>8)&0xff;
	return(OK);
}
