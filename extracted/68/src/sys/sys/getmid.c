/* getmid.c - getmid */

#include <conf.h>
#include <kernel.h>
#include <frame.h>

#define	MACHID	1		/* temporary value for testing		*/

/*------------------------------------------------------------------------
 *  getmid  --  get the machine id for this machine on a given net
 *------------------------------------------------------------------------
 */
SYSCALL	getmid(netid)
	int	netid;
{
	int	id;
	char	ch;

	if (netid < 0 || netid > NNETS)
		return(SYSERR);

	/* replace by query to user or value from ROM if feasible */

	kprintf("Getmid - using machine id %d for network %d\n",
		MACHID, netid);
	return(MACHID);
}
