/* ochk.c - ochk */

#include "define.h"

/*------------------------------------------------------------------------------
 *  ochk  --  precess:	mulu, muls, divu, divs, lea, chk 
 *------------------------------------------------------------------------------
 */
ochk(inst,opcode)
long inst;
register char *opcode;
{
	addtext("\t%s\t",opcode);
	printEA(inst);
	addtext(",%c%D",(*opcode=='l')?'a':'d',(inst>>9)&07);
}
