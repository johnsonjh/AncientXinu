/* exit.c - alarmhandler, inthandler, error, callexit */

#include <sys/ioctl.h>
#include <signal.h>
#include "define.h"

/*------------------------------------------------------------------------------
 * alarmhandler - return from alarm calls
 *------------------------------------------------------------------------------
 */
alarmhandler()
{
	message("Alarm");
	callexit(-3);
}

/*------------------------------------------------------------------------------
 * inthandler -- interrupt processing
 *------------------------------------------------------------------------------
 */
inthandler()
{
	message("Interrupt");
	callexit(2);
}

/*------------------------------------------------------------------------------
 *  error  --  print message to stderr and quit
 *------------------------------------------------------------------------------
 */
error(str1, str2)
	char	*str1, *str2;
{
	fprintf(logfp, str1, str2);
	callexit(-2);
}

/*------------------------------------------------------------------------------
 * callexit - restore ttymodes and exit
 *------------------------------------------------------------------------------
 */
callexit(ret)
int ret;
{
	char	buf[32];

	if ( !ret)
		sendmon("I A\r", NOWAIT);
	message("Exiting");
	restore();
	fclose(logfp);
	exit(ret);
}
