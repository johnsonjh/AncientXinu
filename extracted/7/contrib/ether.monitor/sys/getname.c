/* getname.c - getname */

#include "../h/conf.h"
#include "../h/kernel.h"
#include "../h/proc.h"
#include "../h/q.h"
#include "../h/sleep.h"
#include "../h/network.h"


/*------------------------------------------------------------------------
 *  getname  -  get name of this host and place it where specified
 *------------------------------------------------------------------------
 */
getname(nam)
char	*nam;
{
	char	myaddr[4];

	/* for now, just make up name from IP address */

	getaddr(myaddr);
	sprintf(nam, "knight%d", (0377&myaddr[3])-29);
	return(OK);
}
/* getaddr.c - getaddr */


/*------------------------------------------------------------------------
 *  getaddr  -  obtain this system's complete address (IP address)
 *------------------------------------------------------------------------
 */
getaddr(address)
IPaddr	address;
{
	wait (Arp.rarpsem);
	if (!Net.mavalid)
		sndrarp(ETHER);
	signal(Arp.rarpsem);

	if (!Net.mavalid)
		return(SYSERR);
	blkcopy(address, Net.myaddr, IPLEN);
	return(OK);
}
/* sndrarp.c - sndrarp */


/*------------------------------------------------------------------------
 *  sndrarp  -  broadcast a RARP packet to obtain my IP address
 *------------------------------------------------------------------------
 */
sndrarp()
{
	struct	epacket	*mkarp();
	struct	arppak	*apacptr;
	struct	epacket	*packet;
	int	i;
	int	mypid;
	int	resp;
	IPaddr	junk;
	char	ps;

	mypid = getpid();
	for (i=0 ; i<AR_RTRY && !Net.mavalid ; i++) {
		packet = mkarp(EP_RARP, AR_RREQ, junk, junk);
		if ( ((int)packet) == SYSERR)
			break;
		disable(ps);
		Arp.rarppid = mypid;
		recvclr();
		write(ETHER, packet, EMINPAK);
		resp = recvtim(AR_TIME);
		restore(ps);
		if (resp != TIMEOUT)
			return(OK);
	}
	panic("No response to RARP");
	return(SYSERR);
}
/* mkarp.c - mkarp */


/*------------------------------------------------------------------------
 *  mkarp  -  allocate and fill in an ARP or RARP packet
 *------------------------------------------------------------------------
 */
struct epacket *mkarp(typ, op, spaddr, tpaddr)
short	typ;
short	op;
IPaddr	spaddr;
IPaddr	tpaddr;
{
	register struct	arppak	*apacptr;
	struct	epacket	*packet;

	packet = (struct epacket *) getbuf(Net.netpool);
	if ( ((int)packet) == SYSERR)
		return(SYSERR);
	blkcopy(packet->ep_hdr.e_dest, EBCAST, AR_HLEN);
	packet->ep_hdr.e_ptype = hs2net(typ);
	apacptr = packet->ep_data;
	apacptr->ar_hrd = hs2net(AR_HRD);
	apacptr->ar_prot = hs2net(AR_PROT);
	apacptr->ar_hlen = AR_HLEN;
	apacptr->ar_plen = AR_PLEN;
	apacptr->ar_op = hs2net(op);
	blkcopy(apacptr->ar_sha, eth[0].etpaddr, AR_HLEN);
	blkcopy(apacptr->ar_spa, spaddr, AR_PLEN);
	blkcopy(apacptr->ar_tha, eth[0].etpaddr, AR_HLEN);
	blkcopy(apacptr->ar_tpa, tpaddr, AR_PLEN);
	return(packet);
}
/* recvtim.c - recvtim */


/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
recvtim(maxwait)
int	maxwait;
{
	struct	pentry	*pptr;
	int	msg;
	char	ps;

	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait);
		slnempty = TRUE;
		sltop = & q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);
	return(msg);
}
