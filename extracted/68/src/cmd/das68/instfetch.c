/* instfetch.c - instfetch */

#include "define.h"

/*------------------------------------------------------------------------------
 *  instfetch  --  fetch an instruction or data given its size in bytes
 *------------------------------------------------------------------------------
 */
long instfetch(size)
	register int size;
{	
	register int ans = 0;
	char	string[4];

	while (size--) {
		ans <<= 8;
		ans |= getc(infile) & 0377;
		sprintf(string, "%2.2X", ans&0377);
		strcat(objcode,string);
		dot++;
	}

	strcat(objcode," ");
	return(ans);
}
