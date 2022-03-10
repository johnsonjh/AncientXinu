/* lfinit.c - lfinit */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>

#ifndef	Ndf
#define	Ndf	1
#endif
struct	flblk	fltab[Ndf];

/*------------------------------------------------------------------------
 *  lfinit  --  mark disk file 'device' available at system startup
 *------------------------------------------------------------------------
 */
lfinit(devptr)
struct	devsw	*devptr;
{
	struct	flblk	*flptr;

	devptr->dvioblk = (char *)flptr = &fltab[devptr->dvminor];
	flptr->fl_pid = 0;
	flptr->fl_id = devptr->dvnum;
	return(OK);
}
