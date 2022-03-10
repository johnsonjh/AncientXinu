/* odbcc.c - odbcc */

#include "define.h"
#include <a.out.h>

extern	char	*ccname[];

/*------------------------------------------------------------------------------
 *  odbcc  --  process:		dbdbt, dbf, dbhi, dbls, dbcc, dbcs, dbne, dbeq
				dbvc, dbvs, dbpl, dbmi, dbge, dblt, dbgt, dble
 *------------------------------------------------------------------------------
 */
odbcc(inst,dummy)
long inst;
{
	int	const;

	const = instfetch(2);
	addtext( "\tdb%s\t", ccname[(int)((inst>>8)&017)] );
	printea(0, inst & 07, 1);
	strcat(text, ",");
	pexact(dot+sxtword(const)-2, N_TEXT); 
}
