/* lfinit.c - lfinit */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <disk.h>
#include <file.h>

#ifdef	Ndf
struct	flblk	fltab[Ndf];
#endif

/*------------------------------------------------------------------------
 *  lfinit  --  mark disk file 'device' available at system startup
 *------------------------------------------------------------------------
 */
lfinit(devptr)
struct	devsw	*devptr;
{
	struct	flblk	*flptr;

#ifdef	DEBUG
	dotrace("lfinit", &devptr, 1);
#endif
	devptr->dvioblk = (char *)(flptr = &fltab[devptr->dvminor]);
	flptr->fl_pid = NULLPROC;
	flptr->fl_id = devptr->dvnum;
	return(OK);
}
