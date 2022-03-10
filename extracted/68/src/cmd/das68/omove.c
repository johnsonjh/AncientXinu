/* omove.c - omove */

#include "define.h"

/*------------------------------------------------------------------------------
 *  omove  --  process:	move
 *------------------------------------------------------------------------------
 */
omove(inst, s)
long inst;
char *s;
{
	int size;

	addtext( "\tmov%c\t",*s);
	size = ((*s == 'b') ? 1 : (*s == 'w') ? 2 : 4);
	printEA(inst, size);
	strcat(text, ",");
	printea((inst>>6)&07,(inst>>9)&07,size);
}
