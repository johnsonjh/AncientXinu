/* iblfree.c - iblfree */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  iblfree  --  free a list of iblocks given the number of the first
 *------------------------------------------------------------------------
 */
iblfree(diskdev, iblist)
int	diskdev;
IBADDR	iblist;
{
	IBADDR	ilast;
	struct	iblk	iblock;
	struct	dir	*dirptr;
	int	sem;
	DBADDR	dba;
	int	j;

	if (iblist == IBNULL)
		return(OK);
	dirptr = dsdirec(diskdev);
	ibget(diskdev, iblist, &iblock);
	for (ilast=iblist ; iblock.ib_next!=IBNULL ;) {
		for (j=0 ; j<IBLEN ; j++)
			if ( (dba=iblock.ib_dba[j]) != DBNULL)
				lfsdfree(diskdev, dba);
		ilast = iblock.ib_next;
		ibget(diskdev, ilast, &iblock);
	}
	for (j=0 ; j<IBLEN ; j++)
		if ( (dba=iblock.ib_dba[j]) != DBNULL)
			lfsdfree(diskdev, dba);
	sem = ( (struct dsblk *)devtab[diskdev].dvioblk)->dflsem;
	wait(sem);
	iblock.ib_next = dirptr->d_filst;
	dirptr->d_filst = iblist;
	ibput(diskdev, ilast, &iblock);
	write(diskdev, dskbcpy(dirptr), DIRBLK);
	signal(sem);
	return(OK);
}
