/* suffix.c - suffix */

#include "define.h"

/*------------------------------------------------------------------------------
 *  suffix  --  return character suffix given operand size in bytes
 *------------------------------------------------------------------------------
 */
char suffix(size)
register int size;
{
	return((size==1) ? 'b' : (size==2) ? 'w' : (size==4) ? 'l' : '?');
}
