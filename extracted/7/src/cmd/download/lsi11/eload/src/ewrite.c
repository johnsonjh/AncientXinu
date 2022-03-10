/* ewrite.c - ewrite */

#include "../h/eload.h"
#include "../h/deqna.h"
#include "../h/ether.h"

/*------------------------------------------------------------------------
 *  ewrite - write a single packet to the ethernet
 *------------------------------------------------------------------------
 */
ewrite(myeaddr, wcmd, wpkt, wlen, rcmd, rpkt, rlen )
Eaddr	*myeaddr;
struct	dcmd	*wcmd, *rcmd;
struct	epacket	*wpkt, *rpkt;
int	wlen, rlen;
{
	register struct dqregs	*dqptr;
	int	i, j;

	dqptr = (struct dqregs *) EDEVICE;
	if (wlen < EMINPAK)
		wlen = EMINPAK;
	blkcopy(wpkt->ep_hdr.e_src, myeaddr, EPADLEN);
	while (1) {
		erstart(rcmd, rpkt, rlen);
		ewstart(wcmd, wpkt, wlen, DC_NORM);
		for (j = 0; j < 2; j++)	/* simulate timeout */
			for (i = 0; i < 25000; i++) {
				if (dqptr->d_csr & DQ_XLI)
					return;
				if (dqptr->d_csr & DQ_RLI)
					erstart(rcmd, rpkt, rlen);
			}
		estrt();
	}
}
