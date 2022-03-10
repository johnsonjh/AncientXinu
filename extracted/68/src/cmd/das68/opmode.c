/* opmode.c - opmode */

#include "define.h"

/*------------------------------------------------------------------------------
 *  opmode --  process:	or, sub.bwl, and.bwl, add.bwl
 *------------------------------------------------------------------------------
 */
opmode(inst,opcode)
long inst;
{
	register int opmode = (int)((inst>>6) & 07);
	register int reg = (int)((inst>>9) & 07);
	int size;

	size = (opmode==0 || opmode==4) ?
	1 : (opmode==1 || opmode==3 || opmode==5) ? 2 : 4;
	addtext( "\t%s%c\t", opcode, suffix(size));
	if (opmode>=4 && opmode<=6)
	{
		addtext("d%d,",reg);
		printEA(inst, size);
	}
	else
	{
		printea((inst>>3)&07,inst&07, size);
		addtext(",%c%d",(opmode<=2)?'d':'a',reg);
	}
}
