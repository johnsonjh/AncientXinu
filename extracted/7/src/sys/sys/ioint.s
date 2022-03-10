/* ioint.s - inint, outint */

/* I/O interrupts trap here.  Original PC and PS are on top of the	*/
/* stack upon entry.  Low order 4 bits of the current PS contain the	*/
/* device descriptor.  Interrupts are disabled.				*/

	.globl	_inint,_outint,_intmap
_outint:				/ Output interrupt entry point
	mfps	-(sp)			/ Save device descriptor from PS
	mov	r0,-(sp)		/ Save r0 (csv does not)
	mov	$_intmap+4,r0		/ point r0 to output in intmap
	br	ioint			/ Go do common part of code
_inint:					/ Input interrupt entry point
	mfps	-(sp)			/ Save device code from PS
	mov	r0,-(sp)		/ Save r0 (csv does not)
	mov	$_intmap,r0		/ point r0 to input in intmap
ioint:					/ Code common to input & output
	mov	r1,-(sp)		/ Save r1 (csv does not)
	mov	4(sp),r1		/ Get saved PS in r1
	bic	$177760,r1		/ Mask off device descriptor
	ash	$3,r1			/ pick correct entry in intmap
	add	r1,r0			/ Form pointer to intmap entry
	mov	2(r0),-(sp)		/ Push "code" from intmap as arg
	jsr	pc,*(r0)		/ Call interrupt routine
	mov	2(sp),r1		/ Restore r1 and R0 from stack
	mov	4(sp),r0
	add	$8,sp			/ Pop arg, saved r0, r1, and PS
	rtt				/ Return from interrupt
