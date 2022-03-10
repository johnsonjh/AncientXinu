/* oprint.c - oprint */

#include "define.h"

/*------------------------------------------------------------------------------
 *  oprint  --  process:	reset, stop, rte, trapv, rtr, nop
 *------------------------------------------------------------------------------
 */
oprint(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s",opcode);
}
