/* obranch.c - obranch */

#include "define.h"

char *bname[16] = { 
	"ra", "sr", "hi", "ls", "cc", "cs", "ne",
	"eq", "vc", "vs", "pl", "mi", "ge", "lt", "gt", "le" };

/*------------------------------------------------------------------------------
 *  obranch --  process:	bra, bhi, bhs, bcc, bcs, bne, beq, bvc, bvs,
 *				bpl, bmi, bge, glt, bgt, ble
 *------------------------------------------------------------------------------
 */
obranch(inst,dummy)
long inst;
{
	long disp = inst & 0377;
	char *s; 

	s = "s ";
	if (disp > 127) disp |= ~0377;
	else if (disp == 0) { 
		s = " "; 
		disp = sxtword(instfetch(2))-2; 
	}
	addtext("\tb%s%s\t",bname[(int)((inst>>8)&017)],s);
	pexact(dot+disp, 0);
/*	addtext("0x%x",dot+disp);	*/
}
