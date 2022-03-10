/* ibput.c - ibput */

#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  ibput  --  write an iblock back to disk given its number
 *------------------------------------------------------------------------
 */
ibput(diskdev, inum, loc)
int	diskdev;
IBADDR	inum;
struct	iblk	*loc;
{
	DBADDR	dba;
	char	*buff;
	char	*to, *from;
	int	i;
	int	ibsem;
	int	pluses;

	dba = ibtodb(inum);
	buff = getbuf(dskdbp);
	ibsem = ((struct dsblk *)devtab[diskdev].dvioblk)->dibsem;
	wait(ibsem);
	read(diskdev, buff, dba);
	to = buff + ibdisp(inum);
	from = (char *)loc;
	pluses = FALSE;
	for (i=0 ; i<sizeof(struct iblk) ; i++)
		pluses |= (*to++ = *from++) == '+';
	if (pluses)
		kprintf("ibput(%d, 0x%x, 0x%x) writing pluses\n",
			diskdev, inum, loc);
	write(diskdev, buff, dba);
	signal(ibsem);
	return(OK);
}
