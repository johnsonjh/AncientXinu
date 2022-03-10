/* loadprog.c - loadprog */

#include <a.out.h>
#include "define.h"
#include "regdefs.h"
#define	STACKADDR 0xffc

char Fastbuf[BUFSIZ];

/*------------------------------------------------------------------------------
 * loadprog - load user program over high speed line
 *------------------------------------------------------------------------------
 */

loadprog(userfp, userhdr, loadaddr, fastlength)
FILE	*userfp;
struct	exec	*userhdr;
int	loadaddr;
int	fastlength;
{
	int	bytes;
	int	length;
	char	buf[PACKSIZE];
	char	resp;

	bytes = userhdr->a_text + userhdr->a_data;
	setbuf(stdout, Fastbuf);
	setlong( STACKADDR, (MAXADDR-fastlength-4));
	startmon( MAXADDR-fastlength );

	resp = getchar();
	while ( bytes > 0 ) {
		length = (bytes>PACKSIZE ? PACKSIZE : bytes);
		fread(buf, 1 , length, userfp);
		do {
			sendpack(loadaddr,length,buf);
	                alarm(4);
	                if ( read ( 0, &resp, 1) != 1 ) 
				resp = 'T';
	                alarm(0);
	                if ( resp == NAK )
	                        message("NAK");
		} while ( resp != ACK );
		loadaddr += length;
		bytes -= length;
	}
	sendpack ( loadaddr, 0, buf );
	alarm(4);
	if ( read ( 0, &resp, 1) != 1 ) 
		resp = 'T';
	alarm(0);
	message("Done loading user program");
}
