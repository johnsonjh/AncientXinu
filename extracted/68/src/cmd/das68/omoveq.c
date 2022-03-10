/* omoveq.c - omoveq */

#include "define.h"

/*------------------------------------------------------------------------------
 *  omoveq  --  process:	moveq
 *------------------------------------------------------------------------------
 */
omoveq(inst,dummy)
long inst;
{
	register int data = (int)(inst & 0377);

	if (data > 127) data |= ~0377;
	addtext("\tmoveq\t"); 
	addtext(IMDF, data);
	addtext(",d%D", (inst>>9)&07);
}
