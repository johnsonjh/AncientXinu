/* mon1.c */

#include <kernel.h>
#include <deqna.h>
#include <ether.h>
#include <slu.h>
#include <ip.h>
#include <udp.h>
#include <arp.h>
#include "network.h"

#define	CDEVICE		0177560		/* console device address        */
#define EDEVICE		0174440		/* DEQNA device address		 */
#define ETHINTV		0400		/* DEQNA interrupt vector        */
#define	kputc(x,c)	while((x->ctstat & SLUREADY) == 0); x->ctbuf = c

char	str[80];

main()
{

	struct	dqregs	*dqptr;		/* pointer to DEQNA registers	*/
	struct	dcmd	*dcmptr;	/* pointer to DEQNA command list*/
	struct	epacket	*getbuf();
	struct	epacket	*packet;	/* pointer to Ethernet frame	*/
	struct	arppak	*apacptr;	/* pointer to ARP packet	*/
	struct	ip	*ipptr;		/* pointer to IP part of packet	*/
	struct	udp	*udpptr;	/* pointer to udp part		*/
	struct  data    *dl;
	struct	csr	*console;	/* console line CSR address	*/
	int	*iptr;			/* integer pointer		*/
	int	i, j, len, pool;	/* indexes and length of input	*/
	char	setup[DQ_ROWS][DQ_COLS];/* area for setup packet	*/
	char	secho[sizeof(setup)+4];	/* area for setup packet echo	*/
	char	ps;
	struct	dcmd	ercmd[2];	/* DEQNA read command list	*/
	IPaddr	myaddr;
	IPaddr	dladdr;
	struct	dcmd	ewcmd[2];	/* DEQNA write command list	*/
	Eaddr	myeaddr;		/* my Ethernet address		*/

	/* set up device addresses */


	dqptr = (struct dqregs *) EDEVICE;
	console = (struct csr *)  CDEVICE;

	myaddr[0]=(char)128;
	myaddr[1]=(char)10;
	myaddr[2]=(char)2;
	myaddr[3]=(char)32;

	dladdr[0]=(char)128;
	dladdr[1]=(char)10;
	dladdr[2]=(char)2;
	dladdr[3]=(char)8;
	/* establish read and write buffer descriptor lists */

	disable(ps);
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
	ewstart(&ewcmd[0], setup, sizeof(setup)+2, DC_SETUP);

	/* poll device until setup processed */

	for (dcmptr=ercmd ; dcmptr->dc_st1 == DC_INIT ; )
		;
	/* reset device, leaving it online */

	dqptr->d_csr |=  DQ_REST;
	dqptr->d_csr &= ~DQ_REST;
	dqptr->d_csr &= ~DQ_ELOP;
	dqptr->d_csr |= (DQ_ENBL| DQ_ILOP);

	/* DEQNA has been initialized and is ready to use; need to  */
	/* send ARP packet to advertise physical address before     */
	/* being ready to receive data packets			    */

	pool = mkpool(EMAXPAK,5);

        /* read a ARP or IP packet */
    
	for(packet = getbuf(pool); TRUE;) {
		len = eread(ercmd,packet,sizeof(*packet));
		if(len == SYSERR){
			continue;
		}else switch(net2hs(packet->e_ptype)){
			case EP_ARP:
				continue;
			case EP_IP:

				ipptr = (struct ip *)packet->ep_data;
				udpptr = (struct udp *)ipptr->i_data;

				sprintf(str,"%d.%d.%d.%d  ->  %d.%d.%d.%d\n",ipptr->i_src[0],ipptr->i_src[1],ipptr->i_src[2],ipptr->i_src[3],ipptr->i_dest[0],ipptr->i_dest[1],ipptr->i_dest[2],ipptr->i_dest[3]);
				write(0,str,strlen(str));
				freebuf(packet);
				if (ipptr->i_proto != IPRO_UDP) {
					continue;
				} else{
				continue;

				}
			default:
				continue;
			}
		break;

		}

	restore(ps);
  }

/*------------------------------------------------------------------------
 *  erstart - start an ethernet read operation on the DEQNA
 *------------------------------------------------------------------------
 */

erstart(dcmptr, buf, len)
	struct	dcmd	*dcmptr;
	char	*buf;
	int	len;
{
	struct	dqregs	*dqptr;

	dcmptr->dc_bufh = DC_VALID;
	dcmptr->dc_buf = (unsigned)buf;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr = (struct dqregs *)EDEVICE;
	dqptr->d_rcmd = (unsigned)dcmptr;
	dqptr->d_rcmdh = NULL;
	return;
}



/*------------------------------------------------------------------------
 *  eread - read a single packet from the ethernet
 *------------------------------------------------------------------------
 */
eread(dcmptr, buf, len)
struct	dcmd	*dcmptr;
char	*buf;
int	len;
{


	erstart(dcmptr, buf, len);
	while(dcmptr->dc_st1 == DC_INIT){
		continue;
	}
	if ( (dcmptr->dc_st1 & DC_LUSE) == DC_ERRU) {
		return(SYSERR);
	}
	len = (dcmptr->dc_st1 & DC_HLEN) | (dcmptr->dc_st2 & DC_LLEN);
	len += DC_XLEN;
	return(len);
}


/*-----------------------------------------------------------------------------
 *  ewstart - start an ethernet write operation on the DEQNA
 *-----------------------------------------------------------------------------
 */

ewstart(dcmptr, packet, len, setup)
	struct	dcmd	*dcmptr;
	struct	epacket	*packet;
	int	len;
	int	setup;
{
	register struct	dqregs	*dqptr;

	dqptr = (struct dqregs *)EDEVICE;
	while (! (dqptr->d_csr & DQ_XLI) )
		;
	dcmptr->dc_bufh = DC_VALID | DC_ENDM | setup;
	if (isodd(len))
		dcmptr->dc_bufh |= DC_LBIT;
	dcmptr->dc_buf  = (unsigned)packet;
	dcmptr->dc_len = dqlen(len);
	dcmptr->dc_st1 = dcmptr->dc_st2 = DC_INIT;
	dcmptr->dc_flag = DC_NUSED;
	dqptr->d_wcmd = (unsigned)dcmptr;
	dqptr->d_wcmdh = NULL;
	return;
}


/*------------------------------------------------------------------------
 *  ewrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */
ewrite(dcmptr, myeaddr, packet, len)
struct	dcmd	*dcmptr;
Eaddr	*myeaddr;
struct	epacket	*packet;
int	len;
{
	if (len < EMINPAK)
		len = EMINPAK;
	blkcopy(packet->ep_hdr.e_src, myeaddr, EPADLEN);
	ewstart(dcmptr, packet, len, DC_NORM);
}



/*------------------------------------------------------------------------
 *  blkcopy  -  copy a block of memory form one location to another
 *------------------------------------------------------------------------
 */
blkcopy(to, from, nbytes)
register char	*to;
register char	*from;
register int	nbytes;
{
	while (--nbytes >= 0)
		*to++ = *from++;
	return(OK);
}

 
/*------------------------------------------------------------------------
 *  blkequ  -  return TRUE iff one block of memory is equal to another
 *------------------------------------------------------------------------
 */
blkequ(first, second, nbytes)
register char	*first;
register char	*second;
register int	nbytes;
{
	while (--nbytes >= 0)
		if (*first++ != *second++)
			return(FALSE);
	return(TRUE);
}

