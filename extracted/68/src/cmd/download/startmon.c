/* startmon.c - startmon */

#include "define.h"

/*-----------------------------------------------------------------------------
 *  startmon  --  start execution at a specified address
 *-----------------------------------------------------------------------------
 */
startmon(address)
int	address;
{
	char	buf[16];

	sprintf(buf, "g %08x\r", address);
	sendmon(buf, WAIT);
}
