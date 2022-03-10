/* addtext.c - addtext */

#include "define.h"

/*------------------------------------------------------------------------------
 *  addtext  --  add a string to the disassembled text output
 *------------------------------------------------------------------------------
 */
addtext(control, arg1, arg2, arg3, arg4, arg5, arg6)
	char	*control;
	int	arg1, arg2, arg3, arg4, arg5, arg6;
{
	char	string[100];

	sprintf(string, control, arg1, arg2, arg3, arg4, arg5, arg6);
	strcat(text, string);
}
