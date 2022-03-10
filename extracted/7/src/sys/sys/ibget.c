/* ibget.c - ibget */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <lfile.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  ibget  --  get an iblock from disk given its number
 *------------------------------------------------------------------------
 */
ibget(diskdev, inum, loc)
int	diskdev;
IBADDR	inum;
struct	iblk	*loc;
{
	char	*from, *to;
	int	i;
	char	*buff;

	buff = getbuf(dskdbp);
	read(diskdev, buff, ibtodb(inum));
	from = buff + ibdisp(inum);
	to = (char *)loc;
	for (i=0 ; i<sizeof(struct iblk) ; i++)
		*to++ = *from++;
	freebuf(buff);
}
