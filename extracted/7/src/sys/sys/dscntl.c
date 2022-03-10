/* dscntl.c - dscntl */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dscntl  --  control disk driver/device
 *------------------------------------------------------------------------
 */
dscntl(devptr, func)
	struct	devsw	*devptr;
{
	int	stat;
	char	ps;

	disable(ps);
	switch (func) {

		case DSKSYNC:
			stat = dsksync(devptr);
			break;

		default:
			stat = SYSERR;
			break;
	}
	restore(ps);
	return(stat);
}
