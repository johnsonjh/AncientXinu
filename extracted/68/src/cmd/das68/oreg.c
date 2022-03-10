/* oreg.c - oreg */

#include "define.h"

/*------------------------------------------------------------------------------
 *  oreg  --  	precess:	swap, ext.wl, unlk, move_to_usp, move_from_usp
 *------------------------------------------------------------------------------
 */
oreg(inst,opcode)
long inst;
register char *opcode;
{
	addtext( opcode, (inst & 07));
}
