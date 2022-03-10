/* recvclr.c - recvclr */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  recvclr  --  clear messages, returning waiting message (if any)
 *------------------------------------------------------------------------
 */
SYSCALL	recvclr()
{
	char	ps;
	int	msg;

	disable(ps);
	if ( proctab[currpid].phasmsg ) {	/* existing message?	*/
		proctab[currpid].phasmsg = FALSE;
		msg = proctab[currpid].pmsg;
	} else
		msg = OK;
	restore(ps);
	return(msg);
}
