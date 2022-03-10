|* ioint.s - inint, outint *|

|* I/O interrupts trap here. The device descriptor is on the top of the stack.
|* The corresponding entry in intmap is computed, and the interrupt procedure
|* executed.
	.text
	.globl	_inint,_outint,_intmap
_outint:				| Output interrupt entry point
	movl	a0,sp@-			| Save a0 
	movl	#_intmap+8,a0		| point a0 to output in intmap
	bra	ioint			| Go do common part of code
_inint:					| Input interrupt entry point
	movl	a0,sp@-			| Save a0 
	movl	#_intmap,a0		| Point a0 to input in intmap
ioint:					| Code common to input & output
	movl	d0,sp@-			| Save d0
	movl	sp@(12),d0		| Get saved device descriptor in d0
	asll	#4,d0			| Pick correct entry in intmap
	addl	d0,a0			| Form pointer to intmap entry
	movl	a0@(4),sp@-		| Push "code" from intmap as arg
	movl	a0@, a0			| Do one level of indirection
	jsr	a0@			| Call interrupt routine
	movl	sp@(4),d0		| Restore d0 and a0 from stack
	movl	sp@(8),a0
	addl	#12,sp			| Pop arg, saved a0, and d0 
	rts				| Return to interrupt dispatcher
