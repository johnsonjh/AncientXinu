/* getsymnum.c - getsymnum */

#include "define.h"

extern	struct	relocation_info	*Relocation;
extern	int	Relsize;

/*------------------------------------------------------------------------------
 *  getsymnum  --  find the symbol number of an address if it is defined
 *------------------------------------------------------------------------------
 */
getsymnum()
{
	struct	relocation_info	*r;
	int	length;

	for (r=Relocation; r<&Relocation[Relsize]; r++) {
		if (r->r_address == dot-4)
			return(r->r_symbolnum);
	}
	return(-1);
}
