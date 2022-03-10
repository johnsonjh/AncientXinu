/* x_net.c - x_net */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  x_net  -  (command netstat) print network status information
 *------------------------------------------------------------------------
 */
COMMAND	x_net(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	struct	netq	*nqptr;
	char	str[80];
	int	i;

	sprintf(str,
		"bpool=%d, mutex/cnt=%d/%d, nxt prt=%d, addr %svalid\n",
		Net.netpool, Net.nmutex, scount(Net.nmutex), Net.nxtprt,
		Net.mavalid ? "" : "in");
	write(stdout, str, strlen(str));
	sprintf(str,"Packets: recvd=%d, tossed=%d (%d for overrun)\n",
		Net.npacket, Net.ndrop, Net.nover);
	write(stdout, str, strlen(str));
	for (i=0; i<NETQS; i++) {
		nqptr = &Net.netqs[i];
		if (!nqptr->valid)
			continue;
		sprintf(str,
			"%2d. uport=%4d, pid=%3d, xprt=%2d, size=%2d\n",
			i, nqptr->uport, nqptr->pid, nqptr->xport,
			pcount(nqptr->xport) );
		write(stdout, str, strlen(str));
	}
	return(OK);
}
