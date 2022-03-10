/* pexact.c - pexact, getsymnum */

#include "define.h"
#include <a.out.h>

extern	int	Symsize;
extern	struct	nlist	*Syms, *Symsort;
extern	int	Relsize;
extern	struct	relocation_info	*Relocation;

/*------------------------------------------------------------------------------
 *  pexact  --  find and print symbol label
 *------------------------------------------------------------------------------
 */
pexact(val, type)
register int	val;
register int	type;
{
	register struct	nlist	*s;
	register int	index;

	if (val != 0  ||  (index = getsymnum()) == -1) {
		for (s = Symsort; s < &Symsort[Symsize]; s++) {
			if (val == s->n_value) {
				if (type && (s->n_type != type))
					break;
				addtext("%s",s->n_un.n_name);
				return;
			}
			else if (val < s->n_value) break;
		}
		addtext("0x%x",val);
	} else {
		addtext("%s", Syms[index].n_un.n_name);
	}
}

/*------------------------------------------------------------------------------
 *  getsymnum  --  get symbol index given address in text segment
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
