/* adump.c - adump */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

static	char	*st[4] = {"free","???","gateway","direct"};

/*------------------------------------------------------------------------
 *  adump  -  dump the current contents of the arp cache
 *------------------------------------------------------------------------
 */
adump()
{
	int	arindex;
	struct	arpent	*atabptr;

	kprintf("Routing cache: size=%d, next=%d\n",
		Arp.atabsiz,Arp.atabnxt);
	for (arindex=0; arindex<Arp.atabsiz; arindex++) {
		atabptr = &Arp.arptab[arindex];
		if (atabptr->arp_state != AR_FREE) {
			kprintf(" %2d. Route=%7s, Dev=%2d", arindex,
				st[atabptr->arp_state], atabptr->arp_dev);
			kprintf(" IPaddr=%03d.%03d.%03d.%03d, ",
				atabptr->arp_Iad[0] & 0377,
				atabptr->arp_Iad[1] & 0377,
				atabptr->arp_Iad[2] & 0377,
				atabptr->arp_Iad[3] & 0377);
			kprintf("Ether addr=%02x%02x.%02x%02x.%02x%02x",
				atabptr->arp_Ead[0]&0377,
				atabptr->arp_Ead[1]&0377,
				atabptr->arp_Ead[2]&0377,
				atabptr->arp_Ead[3]&0377,
				atabptr->arp_Ead[4]&0377,
				atabptr->arp_Ead[5]&0377 );
		}
	}
}
