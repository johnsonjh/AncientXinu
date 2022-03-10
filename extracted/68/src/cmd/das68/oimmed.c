/* oimmed.c - oimmed */

#include "define.h"

/*------------------------------------------------------------------------------
 *  oimmed  --  process:	or, and, sub, add, eor, cmp 
 *------------------------------------------------------------------------------
 */
oimmed(inst,opcode) 
long inst;
register char *opcode;
{
	register int size = mapsize(inst);
	long const;

	if (size > 0)
	{
		const = instfetch(size==4?4:2);
		addtext( "\t%s%c\t", opcode, suffix(size));
		addtext( IMDF, const); 
		strcat(text, ",");
		printEA(inst,size);
	}
	else {
		addtext(BADOP, NULL);
	}
}
