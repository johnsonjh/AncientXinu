/* startup.s - start */

DISABLE	=	340			/ PS to disable interrupts

/*----------------------------------------------------------------------
/*  Etherload startup header -- branches to main program after disabling
/*----------------------------------------------------------------------
	.globl	estart
args:
.	=	4.^.			/ space for args
estart:
	mtps	$DISABLE		/ Disable interrupts
	jsr	pc,_eload		/ start eload C program
	halt				/ halt if program returns
	jbr	estart			/ rerun if user types P
