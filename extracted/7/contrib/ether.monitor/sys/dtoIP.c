/* dtoIP.c - dtoIP */

#include <conf.h>
#include <kernel.h>
#include <network.h>

/*------------------------------------------------------------------------
 *  dtoIP  -  convert network address in "decimal dot" notation to IP addr
 *------------------------------------------------------------------------
 */
dtoIP(ip, nad1, nad2, nad3, nad4)
char	*ip;
int	nad1, nad2, nad3, nad4;
{
	*ip++ = LOWBYTE & nad1;
	*ip++ = LOWBYTE & nad2;
	*ip++ = LOWBYTE & nad3;
	*ip++ = LOWBYTE & nad4;
}
