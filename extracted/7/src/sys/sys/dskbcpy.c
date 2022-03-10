/* dskbcpy.c - dskbcpy */

#include <conf.h>
#include <kernel.h>
#include <disk.h>

/*------------------------------------------------------------------------
 *  dskbcpy  --  copy data into a new disk buffer and return its address
 *------------------------------------------------------------------------
 */
dskbcpy(oldbuf)
char	*oldbuf;
{
	int	i;
	char	*newbuf, *to;

	newbuf = to = getbuf(dskdbp);
	for (i=0 ; i<DBUFSIZ ; i++)
		*to++ = *oldbuf++;
	return(newbuf);
}
