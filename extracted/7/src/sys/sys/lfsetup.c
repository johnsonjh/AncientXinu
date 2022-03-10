/* lfsetup.c - lfsetup */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfsetup  --  set up appropriate iblock and data block in memory
 *------------------------------------------------------------------------
 */
lfsetup(diskdev, flptr)
int	diskdev;
struct	flblk	*flptr;
{
	struct	iblk	*ibptr;
	int	displ;
	long	ibrange;
	DBADDR	dba;

	ibrange = (long) (IBLEN * DBUFSIZ);
	ibptr = &flptr->fl_iblk;
	if (flptr->fl_pos < ibptr->ib_byte) {
		flptr->fl_iba = (flptr->fl_dent)->fdiba;
		ibget(diskdev, flptr->fl_iba, ibptr);
	}
	while (ibptr->ib_byte+ibrange <= flptr->fl_pos) {
		if (ibptr->ib_next == IBNULL) {
			ibptr->ib_next = ibnew(diskdev, IBWDIR);
			ibput(diskdev, flptr->fl_iba, ibptr);
			flptr->fl_iba = ibptr->ib_next;
			ibclear(ibptr, (long)ibptr->ib_byte+ibrange);
			ibput(diskdev, flptr->fl_iba, ibptr);
		} else {
			flptr->fl_iba = ibptr->ib_next;
			ibget(diskdev, flptr->fl_iba, ibptr);
		}
	}
	displ = (int) (flptr->fl_pos - ibptr->ib_byte);
	for (flptr->fl_ipnum=0 ; displ>=DBUFSIZ ; displ-=DBUFSIZ)
		flptr->fl_ipnum++;
	flptr->fl_bptr = flptr->fl_buff + displ;
	if ( (dba=ibptr->ib_dba[flptr->fl_ipnum]) == DBNULL) {
		ibptr->ib_dba[flptr->fl_ipnum] = lfsnewd(diskdev,flptr);
		ibput(diskdev, flptr->fl_iba, ibptr);
	} else
		read(diskdev, flptr->fl_buff, dba);
	flptr->fl_dch = FALSE;
}
