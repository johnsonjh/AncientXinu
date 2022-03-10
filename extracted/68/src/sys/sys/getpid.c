/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
#ifdef	DEBUG
	dotrace("getpid", NULL, 0);
#endif
	return(currpid);
}
