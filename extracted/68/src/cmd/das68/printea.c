/* printea.c - printea */

#include "define.h"

/*------------------------------------------------------------------------------
 *  printea  --  print effective address of an instruction
 *------------------------------------------------------------------------------
 */
printea(mode,reg,size)
long mode, reg;
int size;
{
	long index,disp;

	switch ((int)(mode)) {
	case 0:	
		addtext( "d%D",reg);
		break;

	case 1:	
		addtext("a%D",reg);
		break;

	case 2:	
		addtext( "a%D@",reg);
		break;

	case 3:	
		addtext( "a%D@+",reg);
		break;

	case 4:	
		addtext( "a%D@-",reg);
		break;

	case 5:	
		addtext( "a%D@(%D)",reg,sxtword(instfetch(2)));
		break;

	case 6:	
		index = instfetch(2);
		disp = (char)(index&0377);
		addtext("a%d@(%d,%c%D%c)",reg,disp,
		(index&0100000)?'a':'d',(index>>12)&07,
		(index&04000)?'l':'w');
		break;

	case 7:	
		switch ((int)(reg))
		{
		case 0:	
			index = instfetch(2);
			addtext( "%x:w",index);
			break;

		case 1:	
			index = instfetch(4);
			pexact(index, 0);
			break;

		case 2:	
			addtext("pc@(%D)",sxtword(instfetch(2)));
			break;

		case 3:	
			index = instfetch(2);
			disp = (char)(index&0377);
			addtext( "a%D@(%D,%c%D:%c)",reg,disp,
			(index&0100000)?'a':'d',(index>>12)&07,
			(index&04000)?'l':'w');
			break;

		case 4:	
			index = instfetch(size==4?4:2);
			addtext( IMDF, sxtword(index));
			break;

		default:	
			addtext( "???");
			break;
		}
		break;

	default:	
		addtext( "???");
	}
}
