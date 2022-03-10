/* sizmem.s - sizmem */

#include <procreg.h>

/*----------------------------------------------------------------------------
 *  sizmem  -- size memory, place highest valid address of a byte in 
 *		argument. Assumption: executing on kernel stack, interrupts
 *		disabled
 *	       stack: on entry AP+4 contains the address of a longword where
 *		maxaddr should be stored.
 *----------------------------------------------------------------------------
*/

#define	HIGHADDR 0x20000000		/* highest address of int on VAX*/
					/* (below device csr addresses)	*/
	.text
	.align	1
	.globl	_sizmem
_sizmem:
	.word	0x4			/* save register 2 		*/
	movl	*$4, -(sp)		/* push old exception vector	*/
	bisl3	$1, $siztrap, *$4	/* set exception vector to	*/
					/* catch memory exceptions	*/
					/* (with memory mapping 	*/
					/* disabled, the VAX takes a	*/
					/* machine check exception)	*/
					/* NOTE: must handle mach. chk.	*/
					/* on interrupt stack		*/
					/* (hence bits<1,0> == 01)	*/
	clrl	r0			/* r0 holds low address		*/
	movl	$HIGHADDR, r1		/* r1 = highest addr of an int	*/
sizloop:
	cmpl	r0, r1			/* see if low > high		*/
	jgtru	setsize			/* yes: restore & return	*/
	addl3	r0, r1, r2		/* calculate midpoint		*/
	jcc	rotate			/* if carry set, set low bit,	*/
	bisl2	$1, r2			/* soon to be high bit		*/
rotate:
	rotl	$-1, r2, r2		/* div by 2 by shifting right 1	*/
	bicl2	$3, r2			/* address is multiple of 4	*/
	tstl	(r2)+			/* reference what r2 points to	*/
					/* note: if exception taken, r2	*/
					/* is not incremented		*/ 
	movl	r2, r0			/* mid caused no exception -- 	*/
					/* set low to mid address + 4	*/
	jbr	sizloop
setsize:
	addl3	$3, r1, *4(ap)		/* store result			*/
	movl	(sp)+, *$4		/* restore exception vector	*/
	ret				/* return to caller		*/

	.align	2			/* align on longword boundary	*/
siztrap:
					/* machine chk--on intrrpt stk	*/
	subl3	$4, r2, r1		/* midpt caused exception, set	*/
					/* high address to mid - 4	*/
	addl2	(sp)+, sp		/* pop error data from exception*/
	mtpr	$MCESRCLR, $MCESR	/* clear machine check history	*/
	moval	sizloop, (sp)		/* go back to sizloop on rei	*/
	rei				/* back on kernel stack	again	*/
