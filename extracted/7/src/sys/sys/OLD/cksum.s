/* cksum.s - cksum */

/
/ This routine computes the ones complement of the 16 bit ones complement
/ checksum of an array of 16 bit words.  From Noel Chiappa MIT-LCS-CSR)
/
/ C Calling sequence:
/
/	check = cksum(&buf, nwords)
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
~~cksum:
~buf = 4
~nwords = 6
~sum = r0
~bufadr = r1
~nwrds = r2
	mov	r2,	-(sp)		/ Save register 2
	mov	4(sp),	r1		/ Get address of array in r1
	mov	6(sp),	r2		/ Get count of words in r2
	clr	r0			/ Clear r0 for collecting sum

1:	add	(r1)+,	r0		/ add in next word
	adc	r0			/ add in carry bit
	sob	r2,	1b		/ decrement count and continue

	mov	(sp)+,	r2		/ restore r2
	com	r0			/ take 1s complement of result
	rts	pc			/ return to caller

/ This routine computes the 16 bit ones complement checksum of an
/ array of words. It has more setup, but the loop is partially
/ unrolled, so it is faster for longer buffers. The breakeven points
/ are 10 words for instructions and 15 for memory references. The
/ formulas are:
/	Inst	Mem Refs
/ Slow: 6+3n	14+4n
/ Fast: 16+2n	29+3n	(approximately)
/
/
/ Calling sequence:
/
/ int	buf[];
/ int	nwords;
/ int	check;
/
/ check = fcksum(&buf, nwords);
/

ncopies = 8		/ factor by which the loop is unrolled
nmsk = -ncopies		/ used to find out how far into the loop to jump
nshft = -3		/ log of copies, used for divide by shift

.globl	_fcksum

_fcksum:
~~fcksum:

~buf = 6
~nwords = 10

~sum = r0
~bufadr = r1
~nloops = r2
~nleft = r3

	mov	r2,	-(sp)
	mov	r3,	-(sp)
	mov	6(sp),	r1	/ Snarf args
	mov	10(sp),	r2
	clr	r0
	mov	r2,	r3	/ Get ready for disassembly
	ash	$nshft,	r2	/ Figure out how many passes
	inc	r2		/ Plus one for the incomplete one
	bic	$nmsk,	r3	/ Now find out how many in that one
	neg	r3		/ Need to skip the others
	asl	r3		/ Make into an offset
	asl	r3		/ Two insts per word
	jmp	instbl(r3)

1:	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
	add	(r1)+,	r0
	adc	r0
instbl:	sob	r2,	1b

	mov	(sp)+,	r3
	mov	(sp)+,	r2
	com	r0
	rts	pc


/ This routine updates a 1's complement checksum. Its arguments
/ are the old checksum and the difference between the new
/ buffer and the old one; for example, if one modifies a single
/ word, the value passed is (new-old).
/
/ Calling sequence:
/
/ int	oldv, newv, ocheck, ncheck;
/
/ ncheck = acksum(oldv, newv, ocheck);

.globl	_acksum

_acksum:
~~acksum:

~oldv = 2
~newv = 4
~ocheck = 6
	
	mov	6(sp),r0
	add	2(sp),r0
	adc	r0		/ end around carry
	sub	4(sp),r0
	sbc	r0
	rts	pc

.globl _bcopy
_bcopy:
	mov	r1, -(sp)
	mov	r2, -(sp)
	mov	6(sp), r1
	mov	10(sp), r2
	mov	12(sp), r0
	beq	2f
1:	movb	(r1)+, (r2)+
	sob	r0, 1b
	mov	(sp)+, r2
	mov	(sp)+, r1
2:	rts	pc
/
/  Like bcopy only copy in opposite direction.
/
.globl _rbcopy
_rbcopy:
	mov	r1, -(sp)
	mov	r2, -(sp)
	mov	6(sp), r1
	mov	10(sp), r2
	mov	12(sp), r0
	add	r0, r1
	add	r0, r2
	beq	2f
1:	movb	-(r1), -(r2)
	sob	r0, 1b
	mov	(sp)+, r2
	mov	(sp)+, r1
2:	rts	pc
