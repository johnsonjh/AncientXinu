/* sizmem.s - sizmem */

MAXADDR	=	157776			/ Maximum possible mem. address
DISABLE	=	340			/ PS to	disable	interrupts
ENABLE	=	000			/ PS to	enable interrupts
EXCPPC	=	4			/ exception vector address for
EXCPPS	=	6			/ "memory out of range" errors

/*------------------------------------------------------------------------
/*  sizmem  --  size memory, placing highest valid address in _maxaddr
/*------------------------------------------------------------------------
	.globl	sizmem
sizmem:
	mfps	-(sp)			/ save incoming PS
	mtps	$DISABLE		/ disable interrupts
	mov	r0,-(sp)		/ save registers used
	mov	*$EXCPPC,-(sp)		/ save old contents of
	mov	*$EXCPPS,-(sp)		/  exception vector
	mov	$siztrap,*$EXCPPC	/ set up vector to catch memory
	mov	$DISABLE,*$EXCPPS	/ exceptions; disable interrupts
	mov	$MAXADDR,r0		/ set r0 to highest possible loc
sizloop:
	mov	(r0),(r0)		/ reference what r0 points to
	mov	r0,_maxaddr		/ no interrupt - memory	exists.
	mov	(sp)+,*$EXCPPS		/ restore exception vector
	mov	(sp)+,*$EXCPPC
	mov	(sp)+,r0		/ restore r0
	mtps	(sp)+			/ restore PS
	rts	pc			/ return to caller
siztrap:
	add	$4,sp			/ pop interrupted PC and PS
	sub	$2,r0			/ move to next lower address
	jbr	sizloop			/ try again
