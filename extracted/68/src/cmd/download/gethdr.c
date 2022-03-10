/* gethdr.c -- gethdr */

#include "define.h"
#include <a.out.h>

/*------------------------------------------------------------------------------
 * gethdr - read an a.out header
 *------------------------------------------------------------------------------
 */
gethdr(fp, hdr, name)
FILE	*fp;
struct	exec	*hdr;
char	*name;
{
	if ( fread(hdr, sizeof (struct exec), 1, fp) != 1 )
		error("%s: EOF reading a.out header\n",name);
	if ( hdr->a_magic != OMAGIC)
		error("%s: wrong magic number\n",name);
}

