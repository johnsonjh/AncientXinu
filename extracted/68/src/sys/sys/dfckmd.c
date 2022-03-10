/* dfckmd.c - dfckmd */

#include <conf.h>
#include <kernel.h>
#include <disk.h>
#include <file.h>

/*------------------------------------------------------------------------
 *  dfckmd  --  parse file mode argument and generate actual mode bits
 *------------------------------------------------------------------------
 */
dfckmd(mode)
char	*mode;
{
	int	mbits;
	char	ch;

#ifdef	DEBUG1
	dotrace("dfckmd", &mode, 1);
#endif
	mbits = 0;
	while (ch = *mode++)
		switch (ch) {

		    case 'r':	if (mbits&FLREAD) return(SYSERR);
				mbits |= FLREAD;
				break;

		    case 'w':	if (mbits&FLWRITE) return(SYSERR);
				mbits |= FLWRITE;
				break;

		    case 'o':	if (mbits&FLOLD || mbits&FLNEW)
					return(SYSERR);
				mbits |= FLOLD;
				break;

		    case 'n':	if (mbits&FLOLD || mbits&FLNEW)
					return(SYSERR);
				mbits |= FLNEW;
				break;

		    default:	return(SYSERR);
		}
	if (mbits&FLREAD == mbits&FLWRITE)	/* default: allow R + W	*/
		mbits |= (FLREAD|FLWRITE);
	return(mbits);
}
