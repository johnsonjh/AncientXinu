/* dfalloc.c - dfalloc */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  dfalloc  --  allocate a device table entry for a disk file; return id
 *------------------------------------------------------------------------
 */
dfalloc()
{
	int	i;

#ifdef	DEBUG1
	dotrace("dfalloc", NULL, 0);
#endif
	for (i=0 ; i<Ndf ; i++)
		if (fltab[i].fl_pid == 0) {
			fltab[i].fl_pid = getpid();
			return(i);
		}
	return(SYSERR);
}
