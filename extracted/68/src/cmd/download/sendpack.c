/* sendpack.c - sendpack, putw, putch, putwsum putlsum, putchar */

#include "define.h"

union	fourbyte {
	long	l;
	short	s[2];
	char	c[4];
};

/*------------------------------------------------------------------------------
 * sendpack - send a packet to the MC 68000
 *------------------------------------------------------------------------------
 */

long	sum;

sendpack(addr, length, buf)
	long	addr;			/* starting address		*/
	long	length;			/* length of data in bytes	*/
	char	*buf;			/* data output buffer		*/
{
	int	i, j;

	fprintf(logfp,"Sending packet to address 0x%x, of length 0x%x\n",
		addr, length);
	putchar(SOH);
	sum = 0;
	putlsum(addr);
	putlsum(length);
	for ( i=0 ; i<length ; i++ )
		putch(buf[i]);
        putlsum(~sum);
	fflush(stdout);
}

/*------------------------------------------------------------------------------
 * putch - put a character on the high speed line
 *------------------------------------------------------------------------------
 */
putch(ch)
	char	ch;
{
	sum += ch;
	if ( ch == ESC || ch == SOH )
		putchar(ESC);
	putchar(ch);
}

/*------------------------------------------------------------------------------
 * putlsum - write a fourbyte to the high speed line (4 bytes)
 *------------------------------------------------------------------------------
 */
putlsum(x)
long	x;
{
	union	fourbyte y;

	y.l = x;
	putch(y.c[3] & 0xff);
	putch(y.c[2] & 0xff);
	putch(y.c[1] & 0xff);
	putch(y.c[0] & 0xff);
	return;
}
