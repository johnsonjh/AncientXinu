/* printEA.c - printEA */

#include "define.h"

/*------------------------------------------------------------------------------
 *  printEA  --  print effective address of typical instruction
 *------------------------------------------------------------------------------
 */
printEA(ea,size)
long ea;
int size;
{
	printea((ea>>3)&07,ea&07,size);
}
