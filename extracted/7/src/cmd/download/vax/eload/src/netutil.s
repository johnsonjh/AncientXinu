/* netutil.s - hs2net, net2hs, hl2net, net2hl */

/*------------------------------------------------------------------------
 * Utility routines that map host data to/from network byte/word order
 * (needed because host is "Little Endian", network is "Big Endian")
 *------------------------------------------------------------------------
*/
	.text
	.align	1
	.globl	_hl2net
	.globl	_net2hl
_hl2net:			/* Entry point that maps ints,longs	*/
_net2hl:			/*  to/from host/network byte order	*/
	.word	0x0		/* save no registers on entry		*/
	movb	7(ap),r0
	insv	6(ap),$8,$8,r0
	insv	5(ap),$16,$8,r0
	insv	4(ap),$24,$8,r0
	ret

	.align	1
	.globl	_hs2net
	.globl	_net2hs
_hs2net:			/* Entry point that maps shorts to/from	*/
_net2hs:			/*  host/network byte order		*/
	.word	0x0		/* save no registers on entry		*/
	movb	5(ap),r0	/* swap short argument bytes and return	*/
	insv	4(ap),$8,$8,r0
	cvtwl	r0,r0		/* C convention: convert to int		*/
	ret
