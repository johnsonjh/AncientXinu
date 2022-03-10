/* nqalloc.c - nqalloc */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  nqalloc  -  allocate a network demultiplexing queue
 *------------------------------------------------------------------------
 */
nqalloc()
{
	int	i;
	struct	netq	*nqptr;
	char	ps;

	disable(ps);
	for (i=0 ; i<NETQS ; i++) {
		nqptr = &Net.netqs[i];
		if (!nqptr->valid) {
			nqptr->valid = TRUE;
			nqptr->uport = -1;
			nqptr->pid = BADPID;
			restore(ps);
			return(i);
		}
	}
	restore(ps);
	return(SYSERR);
}
