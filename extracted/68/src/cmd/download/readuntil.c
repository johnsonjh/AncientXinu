/* readuntil.c - readuntil */

#include <stdio.h>
#include <errno.h>
#include "define.h"

/*-----------------------------------------------------------------------------
 * readuntil - read from line until some set of characters has been sent
 *-----------------------------------------------------------------------------
 */
readuntil ( ch, buf, time )
	char	*ch;			/* The char. string to expect	*/
	char	*buf;			/* The buffer of incoming chs	*/
	int	time;			/* Seconds till timeout		*/
{
	alarm(time);
	do {
		do {
			*buf = EOS;
			if ( read(0, buf, 1) != 1 )
				error("SUN did not respond\n", NULL);
			fprintf(logfp, "%c",*buf == '\r' ? ' ' : *buf);
		} while ( *buf == EOS || index(ch,*buf++) == NULL );
	} while ( *++ch != EOS);
	*buf = EOS;
	alarm(0);
}
