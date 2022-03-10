/* otrap.c - otrap */

#include "define.h"

/*------------------------------------------------------------------------------
 *  otrap  --  precess:	trap
 *------------------------------------------------------------------------------
 */
otrap(inst,dummy)
long inst;
{
	addtext( "\ttrap\t");
	addtext( IMDF, inst&017);
}
