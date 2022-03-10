/* ibclear.c - ibclear */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  ibclear  --  clear in-core copy of an iblock
 *------------------------------------------------------------------------
 */
ibclear(ibptr, ibbyte)
struct	iblk	*ibptr;
long	ibbyte;
{
	int	i;

	ibptr->ib_byte = ibbyte;
	for (i=0 ; i<IBLEN ; i++)
		ibptr->ib_dba[i] = DBNULL;
	ibptr->ib_next = IBNULL;
}
