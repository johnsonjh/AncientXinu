/* csv.s - csv, cret */

/* C register save:  upon entry here, procedure A has called B, and B	*/
/* has called csv to save registers.  r5 contains return address in B.	*/
/* The stack has old r5, return address in A, and arguments on it.	*/
/* C return: cret (below) is used to restore regs when the called proc.	*/
/* finally exits.  Comments are my best guess; code originally had none.*/

	.globl	csv, cret
/*------------------------------------------------------------------------
/*  csv  --  C register save routine (something to ponder on cold nights)
/*------------------------------------------------------------------------
csv:
	mov	r5,r0			/ r0 not saved at call (C convention)
	mov	sp,r5			/ r5 points to called routine's	frame
	mov	r4,-(sp)		/ push r4 -r2 on stack
	mov	r3,-(sp)
	mov	r2,-(sp)
	jsr	pc,(r0)			/ jsr pushes PC	onto stack goes	to
					/ address in r0	(originally in r5)

/*------------------------------------------------------------------------
/*  cret  --  C register restore routine
/*------------------------------------------------------------------------
cret:
	mov	r5,r2			/ put copy of called frame ptr in r2
	mov	-(r2),r4		/ reload r4 - r2 from start of frame
	mov	-(r2),r3
	mov	-(r2),r2
	mov	r5,sp			/ restore SP
	mov	(sp)+,r5		/ restore r5 saved on stack by call
					/ to csv at procedure entry
	rts	pc			/ return to caller
