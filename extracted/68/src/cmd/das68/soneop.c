/* soneop.c - soneop */

#include "define.h"

/*------------------------------------------------------------------------------
 *  soneop  --  process:	negx, clr, neg, not, tst 
 *------------------------------------------------------------------------------
 */
soneop(inst,opcode)
long inst;
register char *opcode;
{
	register int size = mapsize(inst);

	if (size > 0)
	{
		addtext("\t%s%c\t",opcode,suffix(size));
		printEA(inst);
	}
	else 
		addtext(BADOP, NULL);
}
