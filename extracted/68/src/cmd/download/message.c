/* message.c -- message */

#include "define.h"

/*-----------------------------------------------------------------------------
 * message - conditionally display status message
 *-----------------------------------------------------------------------------
 */
message(str)
char *str;
{
	if ( !A.silent )
		fprintf(logfp, "\n(%s)\n", str);
}

