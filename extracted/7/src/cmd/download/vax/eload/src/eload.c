/* eload.c - eload */

#include "../h/ethdown.h"
#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"
#include "../h/network.h"
#include "../h/ip.h"
#include "../h/udp.h"
#include "../h/arp.h"
#include "../h/dlpack.h"
#include ".version"

/* size of output packet with data */
#define	WTDPKTLEN (EHLEN + IPHLEN + UHLEN + sizeof(struct dl_pack))
/* size of output packet without data */
#define	WTAPKTLEN (EHLEN + IPHLEN + UHLEN + sizeof(struct dl_header))

/*------------------------------------------------------------------------
 *  Etherload - VAX resident piece of Ethernet downloader.  We 
 *		keep variables on the stack so it will run from
 *		ROM.
 */
eload()
{
	char	wbuf[EMAXPAK];		/* buffer for ethernet write	*/
	char	rbuf[EMAXPAK];		/* buffer for ethernet read	*/
	struct	dqregs	*dqptr;		/* pointer to DEQNA registers	*/
	struct	dcmd	*dcmptr;	/* pointer to DEQNA command list*/
	struct	epacket	*epktin;	/* pointer to Ethernet frame	*/
	struct	arppak	*apacptr;	/* pointer to ARP packet	*/
	struct	ip	*ipptr;		/* pointer to IP part of rbuf	*/
	struct	ip	*wipptr;	/* pointer to IP part of wbuf	*/
	struct	udp	*udpptr;	/* pointer to udp part of rbuf	*/
	struct	udp	*wudpptr;	/* pointer to udp part of wbuf	*/
	struct	dl_pack	*prdl;		/* pointer to read command pkt	*/
	struct	dl_pack	*pwdl;		/* pointer to write command pkt	*/
	short	*iptr;			/* to get ethernet phys address	*/
	Bool	idrecvd;		/* true if we've recv'd ID pkt	*/
	int	i, j, len;		/* indexes and length of input	*/
	struct	dcmd	ercmd[2];	/* DEQNA read command list	*/
	struct	dcmd	ewcmd[2];	/* DEQNA write command list	*/
	Eaddr	myeaddr;		/* my ethernet address		*/
	IPaddr	myaddr;			/* my IP address		*/
	char	*from, *to;
	long	delay;			/* time in sec before autostart	*/
	char	*maxaddr;		/* highest byte address		*/

	/* extract physical ethernet address */
	dqptr = (struct dqregs *) EDEVICE;
	for (iptr=(short *) dqptr, i=0 ; i < EPADLEN ; i++)
		myeaddr[i] = LOWBYTE & *iptr++;

	/* establish read and write buffer descriptor lists */
	edcmset(&ercmd[1]);
	edcmset(&ewcmd[1]);

	/* initialize deqna device */
	estrt();
  
	/* pick up IP address argument from (real) locations 0-3 */
	blkcopy(myaddr, ETH_INITPC-4, IPLEN);

	/* size up memory for upload operation */
	sizmem(&maxaddr);

	epktin = (struct epacket *) rbuf;
	ipptr = (struct ip *) epktin->ep_data;
	udpptr = (struct udp *) ipptr->i_data;
	prdl = (struct	dl_pack *) udpptr->u_data;
	wipptr = (struct ip *) ((struct epacket *)wbuf)->ep_data;
	wudpptr = (struct udp *) wipptr->i_data;
	pwdl = (struct dl_pack *) wudpptr->u_data;
		
	idrecvd = FALSE;			/* no ID packet yet	*/

	/* Main loop: read packet and take action */

	while (TRUE) {
		if (eread(ercmd, rbuf, EMAXPAK) == SYSERR)
			continue;
		else if ( net2hs(epktin->ep_hdr.e_ptype ) == EP_ARP ) {
		    apacptr = (struct arppak *) epktin->ep_data;
		    if ( net2hs(apacptr->ar_op) == AR_REQ &&
			 blkequ(myaddr, apacptr->ar_tpa, AR_PLEN) ) {
			/* respond to arp request */
			blkcopy(epktin->ep_hdr.e_dest,
				epktin->ep_hdr.e_src, AR_HLEN);
			apacptr->ar_op = hs2net(AR_RPLY);
			blkcopy(apacptr->ar_sha, myeaddr, AR_HLEN);
			blkcopy(apacptr->ar_spa, myaddr, AR_PLEN);
			blkcopy(apacptr->ar_tha, myeaddr, AR_HLEN);
			/* send ARP packet */
			ewrite(myeaddr, ewcmd, epktin, EMINPAK, ercmd,
				rbuf, EMAXPAK);
			 }
			continue;
		} else if ( net2hs(epktin->ep_hdr.e_ptype) != EP_IP ) {
			continue;
		} else if (ipptr->i_proto != IPRO_UDP) {
			continue;
		} else if (!blkequ(ipptr->i_dest, myaddr, IPLEN)) {
			continue;
		} else if ( net2hs(udpptr->u_dport) != DLUDP) {
			continue;
		}
		/* Have received UDP/IP packet from our download port	*/

		switch (prdl->dl_hdr.dl_op) {	/* execute command dl_op*/

		case	DLDEP:		/* deposit to PDP-11 memory	*/
			to = (char *) net2hl(prdl->dl_hdr.dl_addr);
			from = prdl->dl_pt.dl_data;
			len = net2hs(prdl->dl_hdr.dl_len);
			while (len-- > 0)
				*to++ = *from++;
			/* send ack */
			pwdl->dl_hdr.dl_op = DLACK;
			pwdl->dl_hdr.dl_addr = prdl->dl_hdr.dl_addr;
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			break;

		case DLEXT:	/* send upload packet		*/
			from = (char *) net2hl(prdl->dl_hdr.dl_addr);
			pwdl->dl_hdr.dl_addr = prdl->dl_hdr.dl_addr;
			/* detect out of bounds memory request */
			if (from > maxaddr) {
				pwdl->dl_hdr.dl_op = DLENK;
				udpipwr( myeaddr, idrecvd, wbuf, ewcmd,
					 WTDPKTLEN, rbuf, ercmd,
					 sizeof(rbuf) );
				break;
			}
			to = (char *) pwdl->dl_pt.dl_data;
			len = net2hs(prdl->dl_hdr.dl_len);
			if ( len + (unsigned)from - 1 > (unsigned)maxaddr )
				len = (unsigned)maxaddr-(unsigned)from+1;
			pwdl->dl_hdr.dl_op = DLEAK;
			pwdl->dl_hdr.dl_len = hs2net(len);
			while (len-- > 0)
				*to++ = *from++;
			/* send packet */
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTDPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			break;

		case DLID:	/* ID packet from downloader */
		  if (!idrecvd) {
			idrecvd = TRUE;
			/* setup general purpose output packet */

			/* copy all fields */
			blkcopy( wbuf, rbuf, WTAPKTLEN);
			/* copy ethernet dest. address of downloader */
			blkcopy( ((struct epacket *)wbuf)->ep_hdr.e_dest,
				 ((struct epacket *)rbuf)->ep_hdr.e_src,
				 EPADLEN);
			/* copy ip addresses */
			blkcopy( wipptr->i_dest, ipptr->i_src, IPLEN);
			blkcopy( wipptr->i_src, myaddr, IPLEN);
			/* sequence starts at 0 */
			wipptr->i_id = 0;

			/* fill in udp ports */
			wudpptr->u_sport = udpptr->u_dport;
			wudpptr->u_dport = udpptr->u_sport;
			/* no udp checksum */
			wudpptr->u_ucksum = 0;

			/* fill in downloader packet fields */
			pwdl->dl_hdr.dl_vers = VERSION;
			pwdl->dl_hdr.dl_op = DLRAK;

			/* send ready-ack */
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
		  }
			break;
			
		case DLSRT:	/* start downloaded program		*/
			pwdl->dl_hdr.dl_op = DLQAK;
			/* send 2 quit acks, just to make sure		*/
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			while (! (dqptr->d_csr & DQ_XLI) )
				;	/* wait for write completion	*/
			dqptr->d_csr |= DQ_REST;


			/* go do autostart option */
			delay = net2hl(prdl->dl_pt.dl_srt.dl_delay);
			autostart(delay, net2hl(prdl->dl_pt.dl_srt.dl_entry));
			/* no return */
			break;			/* not needed		*/

		default:			/* send NAK		*/
			pwdl->dl_hdr.dl_op = DLNAK;
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			break;
		}
	}
}
