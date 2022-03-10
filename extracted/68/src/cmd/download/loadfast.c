/* loadfast.c - loadfast */

#include <a.out.h>
#include "define.h"
#include "regdefs.h"

#define	LENADDR	0xffe

/*------------------------------------------------------------------------------
 *  loadfast --  send fast loader to boot loader in reverse order
 *------------------------------------------------------------------------------
 */
loadfast(fastfp, fasthdr)
	FILE	*fastfp;
	struct	exec	*fasthdr;
{

	int	length;
	char	*stack, *stackptr;
	char	buf[128];

	length = fasthdr->a_text + fasthdr->a_data;	
	stack = (char *) malloc(length);	
	stackptr = &stack[length-1];
	while ( stackptr >= stack ) 	/* copy fast loader into memory	*/
		*stackptr-- = (char) getc(fastfp);
	setword(LENADDR,length);	/* length of fast loader	*/
	startmon(BOOTSTART);
        sleep(1);			/* Wait for monitor to get ready*/
	while ( ++stackptr < &stack[length] )
		write(1, stackptr, 1);
	readuntil(">",buf,5);
	return(length);
}
