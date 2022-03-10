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
 *  Etherload - LSI 11/23 resident piece of Ethernet downloader.  We 
 *		keep variables on the stack so it will run from
 *		ROM.  There are two parts: ecopy and erun
 *
 *  Calling conventions:
 *	eload:	main entry point.  It assumes that either MMU is enabled
 *		and code is resident in memory page 7 or MMU is disabled
 *		and code it resident in memory page 0.  It checks MMU to
 *		decide whether to use "ecopy" or "erun"
 *
 *	erun:	assumes itself resident in memory page 7, MMU enabled,
 *		addresses in virtual page 0, interrupts disabled, kernel
 *		mode. This version can load memory pages 0-6 only (up
 *		to 56K).  Will disable MMU and branch to downloaded
 *		program.
 *
 *	ecopy:	assumes itself resident in memory page 0, MMU disabled,
 *		kernel mode.  Also assumes machine has 64K bytes of real
 *		memory.  Ecopy sets up MMU and executes eload once it
 *		finishes copying itself to real	memory page 7.
 *------------------------------------------------------------------------
 */
eload()
{
	/* Test memory management register */
	if ( isodd(*((int *) 0177572)) )
		erun();
	else
		ecopy();
}

/*------------------------------------------------------------------------
 *  ecopy - copy self to memory page 7 and execute from there
 *------------------------------------------------------------------------
 */
ecopy()
{
	int	eload();
	char	*from, *to;
	int	*p;

	p = (int *) 0172340;		/* PAR for page 0		*/
	*p++ = 0;			/* map virtual 0 to real 0	*/
	*p++ = 01600;			/* map virtual 1 to real 7	*/
	p = (int *) 0172356;		/* PAR for page 7		*/
	*p = 0177600;			/* map virtual 7 to I/O		*/

	for (p = (int *)0172300 ; p <= 0172316 ; )
	*p++ = 077406;			/* set PDR-> len=8K, RW access	*/

	p = (int *) 0177572;		/* MMU control register		*/
	*p = 1;				/* Turn on memory management	*/
	from = (char *)0;
	to   = (char *)020000;
	while (from < (char *)020000)	/* copy real page 0 to real 7	*/
		*to++ = *from++;
	/* have now copied myself from real page 0 to real page 7	*/
	p = (int *) 0172340;		/* PAR for virtual page 0	*/
	*p = 01600;			/* set virtual 0 to real 7	*/
	/* SP just jumped from real 0 to real 7, but should not cause	*/
	/* problem because we copied code into real 7 from real 0	*/
	/* However, we have to remember that local variables revert to	*/
	/* values before the copy.					*/
	erun();
}

/*------------------------------------------------------------------------
 *  erun - perform Ethernet download executing from memory page 7
 *------------------------------------------------------------------------
 */
erun()
{
	struct	dqregs	*dqptr;		/* pointer to DEQNA registers	*/
	struct	dcmd	*dcmptr;	/* pointer to DEQNA command list*/
	struct	epacket	*epktin;	/* pointer to Ethernet frame	*/
	struct	arppak	*apacptr;	/* pointer to ARP packet	*/
	struct	ip	*ipptr;		/* pointer to IP part of rbuf	*/
	struct	ip	*wipptr;	/* pointer to IP part of wbuf	*/
	struct	udp	*udpptr;	/* pointer to udp part of rbuf	*/
	struct	udp	*wudpptr;	/* pointer to udp part of wbuf	*/
	struct	dlpack	*prdl;		/* pointer to read command pkt	*/
	struct	dkpack	*pwdl;		/* pointer to write command pkt	*/
	int	*iptr;			/* integer pointer		*/
	Bool	page6mapped;		/* true if page 6 is mapped in	*/
	Bool	idrecvd;		/* true if we've recv'd ID pkt	*/
	int	i, j, len;		/* indexes and length of input	*/
	int	(*q)();			/* used by autostart		*/
	char	wbuf[EMAXPAK];		/* buffer for ethernet write	*/
	char	rbuf[EMAXPAK];		/* buffer for ethernet read	*/
	struct	dcmd	ercmd[2];	/* DEQNA read command list	*/
	struct	dcmd	ewcmd[2];	/* DEQNA write command list	*/
	Eaddr	myeaddr;		/* my ethernet address		*/
	IPaddr	myaddr;			/* my IP address		*/
	char	*from, *to;		/* used to copy bytes		*/
	short	delay;			/* time in sec before autostart	*/

	/* set up remaining memory management (0, 7) already done */
	for (iptr =(int *)0172342, i =00200 ; iptr <= 0172352 ; i += 0200)
		*iptr++ = i;		/* map PARS 1-5 directly to real*/
	*iptr = 00000;			/* map virtual 6 to real 0	*/
	page6mapped = FALSE;		/* page 6 is not directly mapped*/

	for (iptr =(int *)0172300 ; iptr <= 0172314 ; )
		*iptr++ = 077406;	/* set PDRs-> len=8K, RW access	*/

	/* extract physical ethernet address */
	dqptr = (struct dqregs *) EDEVICE;
	for (iptr=(int *) dqptr, i=0 ; i < EPADLEN ; i++)
		myeaddr[i] = LOWBYTE & *iptr++;

	/* establish read and write buffer descriptor lists */
	edcmset(&ercmd[1]);
	edcmset(&ewcmd[1]);

	/* initialize deqna device */
	estrt();
  
	/* pick up IP address argument from (virtual) locations 0-3 */
	blkcopy(&myaddr, 0, IPLEN);

	epktin = (struct epacket *) rbuf;
	ipptr = (struct ip *) epktin->ep_data;
	udpptr = (struct udp *) ipptr->i_data;
	prdl = (struct	dlpack *) udpptr->u_data;
	wipptr = (struct ip *) ((struct epacket *)wbuf)->ep_data;
	wudpptr = (struct udp *) wipptr->i_data;
	pwdl = (struct dlpack *) wudpptr->u_data;

	/* idrecvd indicates whether we've gotten the ID packet yet */
	idrecvd = FALSE;

	/* Main loop: read packet and take action */

	while (TRUE) {
		if ( eread(ercmd, rbuf, EMAXPAK) == SYSERR )
			continue;
		else if ( net2hs(epktin->e_ptype) == EP_ARP ) {
		    apacptr = epktin->ep_data;
		    if ( net2hs(apacptr->ar_op) == AR_REQ &&
			 blkequ(myaddr, apacptr->ar_tpa, AR_PLEN)) {
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
		} else if ( net2hs(epktin->e_ptype) != EP_IP) {
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
			from = &prdl->dl_pt.dl_data;
			len = net2hs(prdl->dl_hdr.dl_len);
			while (len-- > 0) {
				/* if in page 6, map virtual 6 to real 6 */
				if ( (unsigned) to >= 0140000 ) {
					*((int *) 0172354) = 01400;
					page6mapped = TRUE;
				} else if ( (unsigned) to < 020000 ) {
				/* if in page 0, ref page 0 through page 6 */
					*(to+0140000) = *from++;
					to++;
				} else {
				/* somewhere in pages 1..5 */
					*to++ = *from++;
				}
			}
			if ( page6mapped ) {	/* remap virt 6 to real 0 */
				*((int *) 0172354) = 0;
				page6mapped = FALSE;
			}

			/* send ack */
			pwdl->dl_hdr.dl_op = DLACK;
			pwdl->dl_hdr.dl_addr = prdl->dl_hdr.dl_addr;
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			break;

		case DLEXT:	/* send upload packet		*/
			from = (char *) net2hl(prdl->dl_hdr.dl_addr);
			/* if request is out of bounds, send DLENK (NAK) */
			if ( from >= ETH_MAXADDR ) {
				pwdl->dl_hdr.dl_op = DLENK;
				pwdl->dl_hdr.dl_addr = prdl->dl_hdr.dl_addr;
				udpipwr( myeaddr, idrecvd, wbuf, ewcmd,
				     WTAPKTLEN, rbuf, ercmd, sizeof(rbuf) );
				break;
			}
			to = (char *) &pwdl->dl_pt.dl_data;
			len = net2hs(prdl->dl_hdr.dl_len);
			i = 0;
			pwdl->dl_hdr.dl_addr = prdl->dl_hdr.dl_addr;
			pwdl->dl_hdr.dl_op = DLEAK;
			while (i < len && (unsigned)from < ETH_MAXADDR) {
				/* if in page 6, map 6->real 6 */
				if ( (unsigned) from >= 0140000 ) {
					/* map virtual 6 to real 6 */
					*((int *) 0172354) = 01400;
					page6mapped = TRUE;
				}
				/* if ref to page 0, reference thru page 6 */
				if ( ((unsigned) from) < 020000) {
					*to++ = *(from+0140000);
					from++;
				} else {
				/* reference in page1..page5 */
					*to++ = *from++;
				}
				i++;
			}
			if ( page6mapped ) {	/* remap virt 6 to real 0 */
				*((int *) 0172354) = 0;
				page6mapped = FALSE;
			}
			pwdl->dl_hdr.dl_len = hs2net(i);
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

			/* map virtual page 6 to real page 6		*/
			*((int *)0172354) = 01400;

			/* handle autostart options			*/

			iptr = (int *)0157776;	/* top of page 6	*/
			*iptr-- = (short) net2hl(prdl->dl_pt.dl_srt.dl_entry);
			*iptr-- = 0137;		/* jump instruction	*/
			delay = (short) net2hl(prdl->dl_pt.dl_srt.dl_delay);
			if (delay < 0) 
				*iptr-- = 0;	/* halt instruction	*/
			else {
				*iptr-- = 0240;	/* nop instruction	*/
			   while(delay > 0) {
				for (j = 0; j < 2; j++)
					for (i = 0; i < 31999; i++)
						/* empty */;
				delay--;
			   }
			}
			*iptr   = 05;		/* reset instruction	*/
			q = iptr;
			(*q)();
			/* (reset disables memory management)		*/
			break;			/* not needed		*/

		default:			/* send NAK		*/
			pwdl->dl_hdr.dl_op = DLNAK;
			udpipwr( myeaddr, idrecvd, wbuf, ewcmd, WTAPKTLEN,
				rbuf, ercmd, sizeof(rbuf) );
			break;
		}
	}
}
