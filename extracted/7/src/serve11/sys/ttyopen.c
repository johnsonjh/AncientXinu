/* ttyopen.c - ttyopen */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  ttyopen - open tty device and return descriptor (for namespace)
 *------------------------------------------------------------------------
 */
ttyopen(devptr, nam, mode)
struct	devsw	*devptr;
char	*nam;
char	*mode;
{

	/* This routine is not usually used to open tty devices,   */
	/* but is provided so that automatic calls to open do not  */
	/* fail.  It returns SYSERR unless called with a null name */

	if (*nam == '\0')
		return( devptr->dvnum );
	else
		return(SYSERR);
}
