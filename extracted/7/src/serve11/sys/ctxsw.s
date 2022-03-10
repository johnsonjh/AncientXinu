/* ctxsw.s - ctxsw */

/*------------------------------------------------------------------------
/* ctxsw  --  actually perform context switch, saving/loading registers
/*------------------------------------------------------------------------
  / The stack contains three items upon entry to this routine:
  /
  /	SP+4 =>	address of 9 word save area with new registers + PS
  /	SP+2 =>	address of 9 word save area for old registers + PS
  /	SP   =>	return address
  /
  / The saved state consists of: the values of R0-R5 upon entry, SP+2,
  / PC equal to the return address, and the PS (i.e., the PC and SP are
  / saved as if the calling process had returned to its caller).

	.globl	_ctxsw		/ declare the routine name global
_ctxsw:				/ entry point to context switch
	mov	r0,*2(sp)	/ Save old R0 in old register area
	mov	2(sp),r0	/ Get address of old register area
	add	$2,r0		/  in R0; increment to saved pos. of R1
	mov	r1,(r0)+	/ Save registers R1-R5 in successive
	mov	r2,(r0)+	/  locations of the old process
	mov	r3,(r0)+	/  register save area.  (r0)+ denotes
	mov	r4,(r0)+	/  indirect reference and, as a side
	mov	r5,(r0)+	/  effect, incrementing r0 to next word.
	add	$2,sp		/ move sp beyond the return address,
				/   as if a return had occurred.
	mov	sp,(r0)+	/ save stack pointer
	mov	-(sp),(r0)+	/ Save caller's return address as PC
	mfps	(r0)		/ Save processor status beyond registers
	mov	4(sp),r0	/ Pick up address of new registers in R0
				/ Ready to load registers for the new
				/   process and abandon the old stack.
	mov	2(r0),r1	/ Load R1-R5 and SP from the saved area
	mov	4(r0),r2	/   for the new process.
	mov	6.(r0),r3	/ NOTE: dot following a number makes it
	mov	8.(r0),r4	/   decimal; all others are octal
	mov	10.(r0),r5
	mov	12.(r0),sp	/ Have now actually switched stacks
	mov	16.(r0),-(sp)	/ Push new process PS on new process stack
	mov	14.(r0),-(sp)	/ Push new process PC on new process stack
	mov	(r0),r0		/ Finally, load R0 from new area
	rtt			/ Load PC, PS, and reset SP all at once
