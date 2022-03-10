/* getmon.c - getmon, restore */

#include "define.h"
#include <sgtty.h>

struct	sgttyb	oldtty, newtty;

/*------------------------------------------------------------------------------
 * getmon  --  set linemode options and wait for monitor prompt.
 *             executed directly after monitor 'L' command.
 *------------------------------------------------------------------------------
 */

getmon()
{
        char buf[BUFSIZ];

	gtty(0, &oldtty);
	ttyset = TRUE;
	gtty(0, &newtty);
	newtty.sg_flags &= ~(ALLDELAY|ECHO|CRMOD);
	newtty.sg_flags |=  (RAW|CR2|NL2);
	stty(0, &newtty);
	sendmon("\\", NOWAIT);	/* monitor expects '\\' 	*/
	readuntil(">", buf, 10);
}

/*------------------------------------------------------------------------------
 *  restore  --  restore terminal line to previous state
 *------------------------------------------------------------------------------
 */
restore()
{
	if (ttyset)
		stty(0, &oldtty);
}
