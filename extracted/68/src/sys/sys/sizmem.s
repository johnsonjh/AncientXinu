|* sizmem.s - sizmem */

MAXADDR	=	0x40000			| Maximum possible mem. address
DISABLE	=	0x2700
ENABLE	=	0x2000
EXCPPC	=	0x0008

|*------------------------------------------------------------------------
|*  sizmem  --  size memory, placing highest valid address in _maxaddr
|*------------------------------------------------------------------------
	.text
	.globl	_sizmem
_sizmem:
	movw	sr, sp@-
	movl	#DISABLE, sr
	movl	a0, sp@-
	movl	EXCPPC, sp@-
	movl	#siztrap, EXCPPC
	movl	#MAXADDR, a0
sizloop:
	tstl	a0@
	movl	a0, _maxaddr
	movl	sp@+, EXCPPC
	movl	sp@+, a0
	movw	sp@+, sr
	rts
siztrap:
	addl	#14, sp
	subql	#4, a0
	bra	sizloop
