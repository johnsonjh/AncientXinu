/* sendmon.c - sendmon */

#include "define.h"

/*------------------------------------------------------------------------------
 * sendmon  --  send a message to the monitor
 *------------------------------------------------------------------------------
 */
sendmon(msg, wait)
	char *msg;
	Bool wait;
{
	char buf[32], *ptr, tmpstr[2];

	while (*msg!=EOS) {
		write(1, msg, 1);
                if ( wait ) {
                        tmpstr[0] = *msg;
                        tmpstr[1] = EOS;
                        readuntil(tmpstr, buf, 5);
                }
		msg++;
	}
}
