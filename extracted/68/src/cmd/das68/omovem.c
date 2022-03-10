/* omovem.c - omovem, pregmask */

#include "define.h"

/*------------------------------------------------------------------------------
 *  omovem  --  process:	movem
 *------------------------------------------------------------------------------
 */
omovem(inst,dummy)
long inst;
{
	register int i, list = 0, mask = 0100000;
	register int reglist = (int)(instfetch(2));

	if ((inst & 070) == 040)	/* predecrement */
	{
		for(i = 15; i > 0; i -= 2)
		{ 
			list |= ((mask & reglist) >> i); 
			mask >>= 1; 
		}
		for(i = 1; i < 16; i += 2)
		{ 
			list |= ((mask & reglist) << i); 
			mask >>= 1; 
		}
		reglist = list;
	}
	addtext("\tmovem%c\t",(inst&100)?'l':'w');
	if (inst&02000)
	{
		printEA(inst);
		strcat(text, ",");
		pregmask(reglist);
	}
	else
	{
		pregmask(reglist);
		strcat(text, ",");
		printEA(inst);
	}
}

/*------------------------------------------------------------------------------
 *  pregmask  --  generate register mask form movem instruction
 *------------------------------------------------------------------------------
 */
pregmask(mask)
register int mask;
{
	register int i;
	register int flag = 0;

	addtext("#<");
	for (i=0; i<16; i++)
	{
		if (mask&1)
		{
			if (flag) 
				strcat(text, ",");
			else flag++;
			addtext("%c%d",(i<8)?'d':'a',i&07);
		}
		mask >>= 1;
	}
	addtext(">");
}
