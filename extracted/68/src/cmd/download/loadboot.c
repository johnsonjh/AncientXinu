/* loadboot.c - loadboot, getline */

#include "define.h"
#include <a.out.h>

#define	MORE	1
#define NOMORE	0

/*------------------------------------------------------------------------------
 * loadboot  -- load bootstrap loader via SUN Monitor
 *		input state: after monitor prompt
 * 		output state: same
 *------------------------------------------------------------------------------
 */
loadboot(bootfp)
FILE	*bootfp;
{
	char	buf[128];

	while ( getline(bootfp, buf) == MORE) {
		sendmon(buf, WAIT);
		readuntil(">", buf, 3);
	} 
}

static getline(bootfp, buf)
FILE	*bootfp;
char	*buf;
{
	while( (*buf = getc(bootfp)) != EOF)
		if (*buf++ == '\n') {
			*--buf = '\r';
			return(MORE);
		}
	return(NOMORE);
}

