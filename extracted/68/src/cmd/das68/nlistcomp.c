/* nlistcomp.c - nlistcomp */

#include "define.h"
#include <a.out.h>

/*------------------------------------------------------------------------------
 *  nlistcomp  --  return the difference in value of two symbols in symbol table
 *------------------------------------------------------------------------------
 */
nlistcomp(a,b)
struct nlist *a,*b;
{
	return(a->n_value - b->n_value);
}
