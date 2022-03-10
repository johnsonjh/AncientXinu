/* lfread.c - lfread */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>
#include <dir.h>

/*------------------------------------------------------------------------
 *  lfread  --  read from a previously opened disk file
 *------------------------------------------------------------------------
 */
lfread(devptr, buff, count)
struct	devsw	*devptr;
char	*buff;
int	count;
{
	int	done;
	int	ichar;

#ifdef	DEBUG
	dotrace("lfread", &devptr, 3);
#endif
	if (count < 0)
		return(SYSERR);
	for (done=0 ; done < count ; done++)
		if ( (ichar=lfgetc(devptr)) == SYSERR)
			return(SYSERR);
		else if (ichar == EOF ) {	/* EOF before finished */
			if (done == 0)
				return(EOF);
			else
				return(done);
		} else
			*buff++ = (char) ichar;
	return(done);
}
