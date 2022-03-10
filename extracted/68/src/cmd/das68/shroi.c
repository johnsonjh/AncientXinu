/* shroi.c - shroi */

#include "define.h"

char *shro[4] = { "as", "ls", "rox", "ro" };

/*------------------------------------------------------------------------------
 *  shroi  --  process:		asr, asl, roxr, rocl, lsr, lsl, ror, rol
 *------------------------------------------------------------------------------
 */
shroi(inst,ds)
long inst;
char *ds;
{
	int rx, ry;
	char *opcode;
	if ((inst & 0xC0) == 0xC0)
	{
		opcode = shro[(int)((inst>>9)&03)];
		addtext( "\t%s%s\t", opcode, ds);
		printEA(inst);
	}
	else
	{
		opcode = shro[(int)((inst>>3)&03)];
		addtext( "\t%s%s%c\t",opcode, ds, suffix(mapsize(inst)));
		rx = (int)((inst>>9)&07); 
		ry = (int)(inst&07);
		if ((inst>>5)&01) {
			addtext( "d%d,d%d", rx, ry);
		} else {
			addtext( IMDF, (rx ? rx : 8));
			addtext( ",d%d", ry);
		}
	}
}		
