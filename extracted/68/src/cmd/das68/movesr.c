/* movesr.c - omoveto, omovefrom */

#include "define.h"

/*------------------------------------------------------------------------------
 *  omoveto  --  process:	move_to_sr
 *------------------------------------------------------------------------------
 */
omoveto(inst, opcode)
long	inst;
register char	*opcode;
{
	addtext("\tmovw\t");
	printEA(inst);
	addtext(",%s", opcode);
}

/*------------------------------------------------------------------------------
 *  omovefrom  --  process:	move_from_sr
 *------------------------------------------------------------------------------
 */
omovefrom(inst, opcode)
long	inst;
register char	*opcode;
{
	addtext("\tmovw\t%s,", opcode);
	printEA(inst);
}
