/* autostart.c - autostart */

#include "define.h"

/*------------------------------------------------------------------------------
 *  autostart  --  implement startup options
 *------------------------------------------------------------------------------
 */
autostart()
{
	if ( A.autostart ) {
		startmon(A.loadaddr);
		fprintf(logfp, "\n(Start at 0x%x)\n", A.loadaddr);
	}
}
