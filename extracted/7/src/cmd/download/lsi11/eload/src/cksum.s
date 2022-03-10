/* cksum.s - cksum */

/ Compute the ones complement of the 16 bit ones complement checksum of
/  an array of 16 bit words.  Adapted from a version by Noel Chiappa
/
/ C Calling sequence:
/
/	check = cksum(buf, nwords)
/
/ where arguments and return value are declared as:
/
/	int	buf[];		/* array of words to be summed	 */
/	int	nwords;		/* number of 16-bit words in buf */
/	int	check;		/* returned value		 */
/
/
	.text
	.globl	_cksum
_cksum:
	mov	r2,-(sp)		/ Save register 2
	mov	4(sp),r1		/ Get address of buffer in r1
	mov	6(sp),r2		/ Get count of words in r2
	clr	r0			/ Clear r0 for collecting sum

1:	add	(r1)+,r0		/ add in next word
	adc	r0			/ add in carry bit
	sob	r2,1b			/ decrement count and continue

	mov	(sp)+,r2		/ restore r2
	com	r0			/ take 1s complement of result
	rts	pc			/ return to caller
