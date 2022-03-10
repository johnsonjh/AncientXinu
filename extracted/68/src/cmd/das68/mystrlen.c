/* mystrlen.c - mystrlen */

#include "define.h"

/*------------------------------------------------------------------------------
 *  mystrlen  --  return the cursor position after the printing of a sring
 *------------------------------------------------------------------------------
 */

mystrlen(string)
	char	*string;
{
	int	length = 0;

	while (*string++ != NULL) {
		if (*string == '\t')
			length += ( 7 - ((length + 7) % 8));
		else if (*string == '\n')
			length = 0;
		else length++;
	}
	return(length);
}
