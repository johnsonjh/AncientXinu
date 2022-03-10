/* mapsize.c - mapsize */

#include "define.h"

/*------------------------------------------------------------------------------
 *  mapsize  --  return size of instruction
 *------------------------------------------------------------------------------
 */
mapsize(inst)
long inst;
{
	inst >>= 6;
	inst &= 03;
	return((inst==0) ? 1 : (inst==1) ? 2 : (inst==2) ? 4 : -1);
}
