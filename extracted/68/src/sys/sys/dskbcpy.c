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

#ifdef	DEBUG
	dotrace("dskbcpy", &oldbuf, 1);
#endif
	if ((newbuf=to=(char *)getbuf(dskdbp)) == SYSERR) {
		panic("Cannot copy disk buffer (dskbcpy)\n");
		return(SYSERR);
	}
	for (i=0 ; i<DBUFSIZ ; i++)
		*to++ = *oldbuf++;
	return((int)newbuf);
}
