|* syscall.s - syscall

|*-----------------------------------------------------------------------------
|* syscall  --	make an operating system call
|*-----------------------------------------------------------------------------

	.data
	.text
	.globl	_syscall
_syscall:

|*	1)	Save current SR in process table
|*	2)	Change to kernel mode
|*	3)	Move USP to SSP
|*	4)	Call real syscall routine
|*	5)	Move SSP to USP
|*	6)	Restore SR
|*	7)	Return
