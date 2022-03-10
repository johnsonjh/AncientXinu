/* eload.c - eload */

#include "../h/eload.h"
#include "../h/dlpack.h"
#include "../h/deqna.h"
#include "../h/ether.h"
#include "../h/network.h"
#include "../h/slu.h"
#include "../h/ip.h"
#include "../h/udp.h"
#include "../h/arp.h"

/*------------------------------------------------------------------------
 *  Etherload - LSI 11/23 resident piece of Ethernet downloader.  We try
 *		to keep as much as possible on the stack so it run from
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
 *		mode. This version can load memory pages 0-5 only (up
 *		to 48K).  Will disable MMU and branch to downloaded
 *		program.
 *
 *	ecopy:	assumes itself resident in memory page 0, MMU disabled,
 *		interrupts disabled, kernel mode.  Also assumes machine
 *		has 64K bytes of real memory.  Ecopy sets up MMU and
 *		executes eload once it finishes copying itself to real
 *		memory page 7.
 *------------------------------------------------------------------------
 */
eload()
{
	int	*p;

	p = (int *) 0177572;		/* Memory management register	*/
	if ( isodd(*p) )
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

/*T*/	kputc(CDEVICE, 'a');
	p = (int *) 0172340;		/* PAR for page 0		*/
	*p++ = 0;			/* map virtual 0 to real 0	*/
	*p++ = 01600;			/* map virtual 1 to real 7	*/
	p = (int *) 0172356;		/* PAR for page 7		*/
	*p = 0177600;			/* map virtual 7 to I/O		*/
/*T*/	kputc(CDEVICE, 'b');
	p = (int *) 0172300;		/* PDR for page 0		*/
	*p++ = 077406;			/* page 0 len=8K, RW access	*/
	*p++ = 077406;			/* page 1 len=8K, RW access	*/
	*p++ = 077406;			/* page 2 len=8K, RW access	*/
	*p++ = 077406;			/* page 3 len=8K, RW access	*/
	*p++ = 077406;			/* page 4 len=8K, RW access	*/
	*p++ = 077406;			/* page 5 len=8K, RW access	*/
	*p++ = 077406;			/* page 6 len=8K, RW access	*/
	*p++ = 077406;			/* page 7 len=8K, RW access	*/
/*T*/	kputc(CDEVICE, 'c');

	p = (int *) 0177572;		/* MMU control register		*/
	*p = 1;				/* Turn on memory management	*/
/*T*/	kputc(CDEVICE, 'd');
	from = (char *)0;
	to   = (char *)020000;
	while (from < (char *)020000)	/* copy real page 0 to real 7	*/
		*to++ = *from++;
	/* have now copied myself from real page 0 to real page 7	*/
	p = (int *) 0172340;		/* PAR for virtual page 0	*/
	*p = 01600;			/* set virtual 0 to real 7	*/
/*T*/	kputc(CDEVICE, 'e');
	/* SP just jumped from real 0 to real 7, but should not cause	*/
	/* problem because we copied code into real 7 from real 0	*/
	/* However, we have to remember that local variables revert to	*/
	/* values before the copy.					*/
/*T*/	kputc(CDEVICE, 'f');
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
	struct	epacket	*packet;	/* pointer to Ethernet frame	*/
	struct	arppak	*apacptr;	/* pointer to ARP packet	*/
	struct	ip	*ipptr;		/* pointer to IP part of packet	*/
	struct	udp	*udpptr;	/* pointer to udp part		*/
	struct	dlpack	*dlptr;		/* pointer to downloader packet	*/
	struct	csr	*console;	/* console line CSR address	*/
	int	*iptr;			/* integer pointer		*/
	int	i, j, len;		/* indexes and length of input	*/
	char	buffer[EMAXPAK];	/* general buffer for reading	*/
	char	setup[DQ_ROWS][DQ_COLS];/* area for setup packet	*/
	char	secho[sizeof(setup)+4];	/* area for setup packet echo	*/
	struct	dcmd	ercmd[2];	/* DEQNA read command list	*/
	struct	dcmd	ewcmd[2];	/* DEQNA write command list	*/
	Eaddr	myeaddr;		/* my Ethernet address		*/
	IPaddr	myaddr;			/* my IP address		*/
	IPaddr	dladdr;			/* downloader's IP address	*/
	short	dludp;			/* downloader's UDP port	*/
	char	*from, *to;
	int	(*q)();

	/* set up remaining memory management (0, 7) already done */

	iptr = (int *)0172342;		/* PAR for page 1		*/
	*iptr++ = 00200;		/* map virtual 1 to real 1	*/
	*iptr++ = 00400;		/* map virtual 2 to real 2	*/
	*iptr++ = 00600;		/* map virtual 3 to real 3	*/
	*iptr++ = 01000;		/* map virtual 4 to real 4	*/
	*iptr++ = 01200;		/* map virtual 5 to real 5	*/
	*iptr++ = 00000;		/* map virtual 6 to real 0	*/

	iptr = (int *)0172300;		/* PDR for page 1		*/
	*iptr++ = 077406;		/* page 0 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 1 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 2 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 3 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 4 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 5 len=8K, RW access	*/
	*iptr++ = 077406;		/* page 6 len=8K, RW access	*/

	/* set up device addresses */

	dqptr = (struct dqregs *) EDEVICE;
	console = (struct csr *)  CDEVICE;
/*T*/	kputc(console, '0');

	/* Pick up arguments from (real) locations 0-9 */

	blkcopy(&myaddr, 0140000, IPLEN);
	blkcopy(&dladdr, 0140004, IPLEN);
	blkcopy(&dludp,  0140010, sizeof(short));

	/* establish read and write buffer descriptor lists */

	dcmptr = &ercmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = NULL;
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;

	dcmptr = &ewcmd[1];
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf = NULL;
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;

	/* initialize device */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_vect = ETHINTV;

	/* extract physical ethernet address and setup device for it	*/

	for (iptr=(int *)dqptr ,i=0 ; i<EPADLEN ; i++)
		myeaddr[i] = LOWBYTE & *iptr++;

	for (i=0 ; i < (DQ_ROWS>>1) ; i++) {
		setup[i+DQ_SETD][0] = setup[i][0] = 0;
		setup[i+DQ_SETD][1] = setup[i][1] = i<EPADLEN ? DQ_BCST : 0;
		for (j=2 ; j<DQ_COLS ; j++)
			setup[i+DQ_SETD][j] = setup[i][j] = 
				i<EPADLEN ? myeaddr[i] : 0;
	}
	erstart(&ercmd[0], secho, sizeof(secho));
	ewstart(&ewcmd[0], setup, sizeof(setup), DC_SETUP);
/*T*/	kputc(console,'1');

	/* poll device until setup processed */

/*T*/	kputc(console,'2');
	for (dcmptr=ercmd ; dcmptr->dc_st1 == DC_INIT ; )
		;
/*T*/	kputc(console,'3');

	/* reset device, leaving it online */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_csr &= ~DQ_ELOP;
	dqptr->d_csr |= (DQ_ENBL| DQ_ILOP);
/*T*/	kputc(console, '4');

	/* DEQNA has been initialized and is ready to use; need to  */
	/* send ARP packet to advertise physical address before     */
	/* being ready to receive data packets			    */

	packet = (struct epacket *) buffer;

	/* manufacture ARP packet with my IP and Ether addresses */
	blkcopy(packet->ep_hdr.e_dest, EBCAST, AR_HLEN);
	blkcopy(packet->ep_hdr.e_src, myeaddr, AR_HLEN);
	packet->ep_hdr.e_ptype = hs2net(EP_ARP);
	apacptr = packet->ep_data;
	apacptr->ar_hrd = hs2net(AR_HRD);
	apacptr->ar_prot = hs2net(AR_PROT);
	apacptr->ar_hlen = AR_HLEN;
	apacptr->ar_plen = AR_PLEN;
	apacptr->ar_op = hs2net(AR_RPLY);
	blkcopy(apacptr->ar_sha, myeaddr, AR_HLEN);
	blkcopy(apacptr->ar_spa, myaddr, AR_PLEN);
	blkcopy(apacptr->ar_tha, myeaddr, AR_HLEN);
	blkcopy(apacptr->ar_tpa, myaddr, AR_PLEN);
	/* send ARP packet */
	ewrite(ewcmd, myeaddr, packet, EMINPAK);
/*T*/	kputc(console,'5');
	kputc(console, 'R');

	/* Main loop: read packet and take action */

	ipptr = (struct ip *)packet->ep_data;
	udpptr = (struct udp *)ipptr->i_data;
	dlptr = (struct	dlpack *) udpptr->u_data;
	while (TRUE) {
		len = eread(ercmd, buffer, EMAXPAK);
/*T*/		kputc(console,'6');
		if (len == SYSERR) {
/*T*/			kputc(console, 'l');
			continue;
		} else if ( net2hs(packet->e_ptype) != EP_IP) {
/*T*/			kputc(console, 'i');
			continue;
		} else if (ipptr->i_proto != IPRO_UDP) {
/*T*/			kputc(console, 'u');
			continue;
		} else if (!blkequ(ipptr->i_src, dladdr, IPLEN)) {
/*T*/			kputc(console, 's');
			continue;
		} else if (!blkequ(ipptr->i_dest, myaddr, IPLEN)) {
			kputc(console, 'd');
			continue;

		} else if ( net2hs(udpptr->u_sport) != dludp) {
			kputc(console, 'p');
			continue;
		}

		/* Have received valid UDP/IP packet from our download	*/
		/* port on the correct machine.  ACK and then honor it.	*/

		kputc(console, 'A');
		if ((to = (char *)net2hs(dlptr->dl_addr)) != DLCMD) {
/*T*/			kputc(console,'7');
			/* copy dlptr->dl_len bytes from dlptr->dl to	*/
			/* location given by i but translate refs in	*/
			/* page 0 to refs in virtual page 6		*/
			from = (char *) &dlptr->dl;
			len = net2hs(dlptr->dl_len);
			while (len-- > 0) {
				if ( ((unsigned)to) < 020000) {
					*(to+0140000) = *from++;
					to++;
				} else {
					*to++ = *from++;
				}
			}
/*T*/			kputc(console,'8');
		} else { /* Command packet to start prog execution	*/
/*T*/			kputc(console,'9');

			/* handle command packet */

			iptr = (int *)0172354;	/* PAR for virtual 6	*/
			*iptr = 0140000;	/* map Vir. 6 to real 6	*/
			iptr = (int *)0157776;	/* top of page 6	*/
			*iptr-- = dlptr->dl.dl.dl_regs[7];
			*iptr-- = 0137;		/* jump instruction	*/
			*iptr   = 05;		/* reset instruction	*/
			q = iptr;
			(*q)();
			/* This is the easiest way to code a jump in C	*/
			/* It should be cleaned up.  Maybe put in the	*/
			/* startup routine... Can we make it the action	*/
			/* if eload returns?				*/

			/* have now gone off into another universe to	*/
			/* execute the program we just downloaded	*/
		}
	}
}
