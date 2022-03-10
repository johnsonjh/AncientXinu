/* oneop.c - oneop */

#include "define.h"

/*------------------------------------------------------------------------------
 *  oneop  --  process:		nbcd, jsr, jmp 
 *------------------------------------------------------------------------------
 */
oneop(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s\t",opcode);
	printEA(inst);
}
