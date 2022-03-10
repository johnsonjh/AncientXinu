/* rarp_in.c - rarp_in */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>
#include <serve11.h>

/*------------------------------------------------------------------------
 *  rarp_in  -  handle RARP packet coming in from Ethernet network
 *------------------------------------------------------------------------
 */
rarp_in(packet, device)
struct	epacket	*packet;
int	device;
{
	char	ps;
	int	pid;
	int	ret;
	int	i, j;
	Bool	done;
	struct	arppak	*apacptr;
	struct	etblk	*etptr;

	apacptr = (struct arppak *) packet->ep_data;
	if (net2hs(apacptr->ar_op) == AR_RRLY) {
		etptr = (struct etblk *)devtab[device].dvioblk;
		if ( blkequ(apacptr->ar_tha,etptr->etpaddr,EPADLEN) ) {
			blkcopy(Net.myaddr, apacptr->ar_tpa, IPLEN);
			netnum(Net.mynet, Net.myaddr);
			disable(ps);
			Net.mavalid = TRUE;
			pid = Arp.rarppid;
			if (!isbadpid(pid)) {
				Arp.rarppid = BADPID;
				send(pid, OK);
			}
			restore(ps);
		}
		ret = OK;
	} else if (! serinfo.ser_rarp)
		ret = SYSERR;
	else {	/* implement RARP server */
		for (i=0 ; i<Arp.rtabsiz ; i++) {
			if (blkequ(Arp.rarptab[i].rarp_ea,
				apacptr->ar_sha, EPADLEN))
				break;
		}
		if (i >= Arp.rtabsiz) {
			if (Arp.rtabsiz >= AR_RTAB) {
				freebuf(packet);
				kprintf("RARP overflow\n");
				return(SYSERR);
			}
			i = Arp.rtabsiz++;
			blkcopy(Arp.rarptab[i].rarp_ea, apacptr->ar_sha,
				EPADLEN);

			/* make up an ip address based on my address and
			   the table position of the entry (i). This method
			   is kind of a hack, consider changing to some other
			   scheme.					*/
			blkcopy(Arp.rarptab[i].rarp_ip, Net.myaddr, IPLEN);
			Arp.rarptab[i].rarp_ip[3] += (i + 1) & LOWBYTE;

			/* look to see that this addr does not already exist */
			done = FALSE;
			while (!done) {
				for (j=0 ; j<Arp.rtabsiz-1 ; j++) {
					if (blkequ(Arp.rarptab[i].rarp_ip,
					    Arp.rarptab[j].rarp_ip, IPLEN)) {
						Arp.rarptab[i].rarp_ip[3] += (i + 1) & LOWBYTE;
						break;
						}
					}
				if (j == Arp.rtabsiz-1)
					done = TRUE;
				}
			}
		if (serinfo.ser_name)
			/* put this machine in name table */
			putname(Arp.rarptab[i].rarp_ip);
		apacptr->ar_op = hs2net(AR_RRLY);
		blkcopy(apacptr->ar_tha, apacptr->ar_sha, EPADLEN);
		blkcopy(apacptr->ar_tpa, Arp.rarptab[i].rarp_ip, IPLEN);
		blkcopy(apacptr->ar_sha, etptr->etpaddr, EPADLEN);
		blkcopy(apacptr->ar_spa, Net.myaddr, IPLEN);
		blkcopy(packet->ep_hdr.e_dest, apacptr->ar_tha, EPADLEN);
		send(Arp.rarpid, packet);
		ret = OK;
	}
	freebuf(packet);
	return(ret);
}
