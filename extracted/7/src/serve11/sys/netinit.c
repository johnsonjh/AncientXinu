/* netinit.c - netinit */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <network.h>
#include <serve11.h>

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
	if (!serinfo.ser_rarp)
		Net.mavalid = FALSE;
	if (!serinfo.ser_name) 
		Net.mnvalid = FALSE;
	dot2ip(Net.gateway, GATEWAY);
	Net.nxtprt = ULPORT;
	Net.nmutex = screate(1);
	Net.npacket = Net.ndrop = Net.nover = 0;
	return(OK);
}

struct	netinfo	Net;
