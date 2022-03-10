/* oscc.c - oscc */

#include "define.h"

extern	char	*ccname[];

/*------------------------------------------------------------------------------
 *  oscc  --  process:		sst, sf, shi, sls, scc, scs, sne, seq, svc,
 *				svs, spl, smi, sge, slt, sgt, sle
 *------------------------------------------------------------------------------
 */
oscc(inst,dummy)
long inst;
{
	addtext( "\ts%s\t",ccname[(int)((inst>>8)&017)]);
	printEA(inst, 1);
}
