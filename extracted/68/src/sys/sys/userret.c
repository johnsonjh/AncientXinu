/* userret.c - userret */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 * userret  --  entered when a process exits by return
 *------------------------------------------------------------------------
 */
userret()
{
#ifdef	DEBUG
	dotrace("userret",NULL, 0);
#endif
	kill( getpid() );
}
