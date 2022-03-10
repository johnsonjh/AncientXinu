/* biti.c - biti */

#include "define.h"

char *bit[4] = { "btst", "bchg", "bclr", "bset" };

/*------------------------------------------------------------------------------
 *  biti  --  process:		btst, bchg, bclr, bset
 *------------------------------------------------------------------------------
 */
biti(inst, dummy)
	long inst;
{
	addtext( "\t%s\t", bit[(int)((inst>>6)&03)]);
	if (inst&0x0100) {
		addtext( "d%D,", inst>>9);
	} else { 
		addtext( IMDF, instfetch(2)); 
	}
	strcat(text, ",");
	printEA(inst);
}
