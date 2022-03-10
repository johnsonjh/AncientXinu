/* oquick.c - oquick */

#include "define.h"

/*------------------------------------------------------------------------------
 *  oquick  --  process:	addq, subq
 *------------------------------------------------------------------------------
 */
oquick(inst,opcode)
long inst;
register char *opcode;
{
	register int size = mapsize(inst);
	register int data = (int)((inst>>9) & 07);

	if (data == 0) data = 8;
	if (size > 0)
	{
		addtext("\t%s%c\t", opcode, suffix(size));
		addtext(IMDF, data); 
		strcat(text, ",");
		printEA(inst);
	}
	else addtext(BADOP, NULL);
}
