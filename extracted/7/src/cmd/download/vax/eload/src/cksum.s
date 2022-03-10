/* cksum.s - cksum */

/*
 *----------------------------------------------------------------------------
 * cksum -- compute ones complement of 16-bit ones complement checksum
 *----------------------------------------------------------------------------
 * This routine computes the one's complement of the 16 bit two's complement
 * checksum of an array of words, and is thus suitable for computing the
 * checksum of various levels of the DARPA InterNet protocols.
 *
 *  C Calling sequence:
 *
 *	check = cksum(&buf, nwords)
 *
 *  where arguments are declared as:
 *
 *	int	buf[];		 array of words to be summed
 *	int	nwords;		 number of 16-bit words in buf
 *	int	check;		 returned value
*/
 
	.align	1
	.text
	.globl	_cksum
_cksum:
	.word	0x4			/* save r2			*/
	movl	4(ap),	r1		/* r1 = address of buf		*/
	movl	8(ap),	r2		/* r2 = nwords			*/
	clrl	r0
loop:					/* no  ==> compute cksum	*/
	addw2	(r1)+,	r0
	bcc	decrmnt			/* no carry ==> continue	*/
	incw	r0			/* add carry to r0		*/
decrmnt:
	sobgtr	r2,	loop		/* not done? go back to loop	*/
	mcomw	r0,	r0		/* compute one's complement	*/
	ret
