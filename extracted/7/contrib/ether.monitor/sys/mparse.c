/* mparse.c - mparse */

#include <conf.h>
#include <kernel.h>
#include "../h/mshell.h"

/*------------------------------------------------------------------------
 *  mparse  -  simplistic, ad hoc parser for command line
 *------------------------------------------------------------------------
 */
mparse(line)
char	*line;
{
	char	**tokptr;
	int	ntok, i;
	char	*p;
	char	ch;
	char	*to;

	to = &Msh.margstr;
	tokptr = &Msh.mtokens[ntok = 0];
	for (p=line; *p!='\n' && *p!='\0' && ntok<MMAXTOK; ){
		while ( (ch = *p) == ' ')	/* skip leading blanks	*/
			p++;
		if (ch=='\0' || ch=='\n')      /* end of line or string */
			return(ntok);
		*tokptr++ = to;
		Msh.mtoktyp[ntok++] = ch;
		*to++ = *p++;
		if (ch != '|'){
			while ((ch = *p) != '\n' && ch != '\0'                                                  && ch != '|' && ch != ' ')
				*to++ = *p++ ;
		}
		*to++ = NULLCH;
	}
	return(ntok);
}
