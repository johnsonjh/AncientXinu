/* lfwrite.c - lfwrite */

#include <conf.h>
#include <kernel.h>

/*------------------------------------------------------------------------
 *  lfwrite  --  write 'count' bytes onto a local disk file
 *------------------------------------------------------------------------
 */
lfwrite(devptr, buff, count)
struct	devsw	*devptr;
char	*buff;
int	count;
{
	int	i;

	if (count < 0)
		return(SYSERR);
	for (i=count; i>0 ; i--)
		if (lfputc(devptr, *buff++) == SYSERR)
			return(SYSERR);
	return(count);
}
