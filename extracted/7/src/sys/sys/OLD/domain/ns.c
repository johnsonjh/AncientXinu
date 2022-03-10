/* getInetAddr.c - getInetAddr  */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <nserver.h>

/*
 
 *  GetInetAddr  --  domain name resolver for normal queries
 *              (i.e. given a host name returns the Internet address )
 
 *
 */
SYSCALL getInetAddr (name, ipaddr)
char * name;
IPaddr ipaddr;

{
	int dev, i, j;   
        char * target, *size, *buff, c; 
	struct iq * iqptr; 


	/* open the NETWORK pseudodevice to contact the name server */

	if ((dev = open(NETWORK, N_SERVERADDR, ANYLPORT)) == SYSERR ||
             control(dev, DG_SETMODE, DG_DMODE /* |DG_TMODE */) == SYSERR)
		return(SYSERR);


	if ((buff = getmem(PACKETLGTH)) == SYSERR)
		return(SYSERR);
	iqptr = (struct ip *) buff;
	blkcopy(buff, MSG_HEADER, HEADER_LGTH);   /* message header */
        target =  buff + HEADER_LGTH;
	size = target++;
	do {                                      /* change name to     */
		c = *name++;                      /* Internet format    */
                if (c != '.' && c != '\0')  *target++ = c;
		else {
			*size = target - size - 1;
			size = target++;
			}
		} while (c != '\0');
	*size = '\0';
        *target++ = 0;
        *target++ = 1;
	*target++ = 0;
	*target++ = 1;

	/* send query to domain name server  */

        control (dev, DG_CLEAR, 0);
	write(dev, buff, target - iqptr);

	/* receive response of query */
	if ((j = read(dev, buff, PACKETLGTH)) != TIMEOUT) 
		if ((iqptr->rcode|00) || (iqptr->ancount <= 0))
			j = SYSERR;
		else {
			while (*target != '\0')          /* skip head of R.R. */
				if (0x00C0 & *target)
					*++target = '\0';
				else target += *target + 1;
			target += 2;
			if (*target != 1)
				target = buff + j - 4;   /* address in last 4 */
			else target += 9;                /* skip head of R.R. */	
        		for (i = 0; i < 4; )
                		ipaddr[i++] = *target++;
			j = OK;
			}
	freemem(buff, PACKETLGTH);
        close(dev);
        return(j);
}

