/* olink.c - olink */

#include "define.h"

/*------------------------------------------------------------------------------
 *  olink  --  process:	link
 *------------------------------------------------------------------------------
 */
olink(inst,dummy)
long inst;
{
	addtext( "\tlink\ta%D,", inst&07);
	addtext( IMDF, sxtword(instfetch(2)));
}
