/* extend.c - extend */

#include "define.h"

/*------------------------------------------------------------------------------
 *  extend  --  process:	sbcd, subx, cmpm, exg, abcd, addx
 *------------------------------------------------------------------------------
 */
extend(inst, opcode)
	long	inst;
	char	*opcode;
{
	register int size = mapsize(inst);
	int ry = (inst&07), rx = ((inst>>9)&07);
	char c;

	c = ((inst & 0x1000) ? suffix(size) : ' ');
	addtext( "\t%s%c\t", opcode, c);
	if (*opcode == 'e')
	{
		if (inst & 0x0080) {
			addtext( "d%D,a%D", rx, ry);
		}
		else if (inst & 0x0008) {
			addtext( "a%D,a%D", rx, ry);
		} else {
			addtext( "d%D,d%D", rx, ry);
		}
	} else if ((inst & 0xF000) == 0xB000) {
		addtext( "a%D@+,a%D@+", ry, rx);
	} else if (inst & 0x8) {
		addtext( "a%D@-,a%D@-", ry, rx);
	} else {
		addtext( "d%D,d%D", ry, rx);
	}
}
