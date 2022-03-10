/* rserver.c - rserver */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  rserver  -  output side of RARP server
 *------------------------------------------------------------------------
 */
rserver()
{
	struct	epacket	*packet;

	while (TRUE) {
		packet = (struct epacket *) receive();
		write(ETHER, packet, EMINPAK);
	}
}
