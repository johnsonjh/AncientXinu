/* netinit.c - netinit */

#include "../h/conf.h"
#include "../h/kernel.h"
#include "../h/sleep.h"
#include "../h/network.h"

/*------------------------------------------------------------------------
 *  netinit  -  initialize network data structures
 *------------------------------------------------------------------------
 */
netinit()
{
	struct	netq	*nqptr;
	int	i;

	/* Initialize pool of network buffers and rest of Net structure	*/

	if (clkruns == FALSE)
		panic("net: no clock");
	Net.netpool = mkpool(EMAXPAK, NETBUFS);
	for (i=0 ; i<NETQS ; i++) {
		nqptr = &Net.netqs[i];
		nqptr->valid = FALSE;
		nqptr->uport = -1;
		nqptr->xport = pcreate(NETQLEN);
	}
	Net.mavalid = FALSE;
	dtoIP(&Net.gateway, NETGATE);
	Net.nxtprt = ULPORT;
	Net.nmutex = screate(1);
	Net.npacket = Net.ndrop = Net.nover = 0;
	return(OK);
}

struct	netinfo	Net;
