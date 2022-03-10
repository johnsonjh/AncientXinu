|* lowcore.s - loinit

|*------------------------------------------------------------------------
|* loinit  -  map of dispatch table and interrupt dispatcher 
|*------------------------------------------------------------------------
	.data
	.globl	_disptab
_disptab:
	.long	panic 
	.long	0x0000
	.long	panic 
	.long	0x0001
	.long	panic 
	.long	0x0002
	.long	panic 
	.long	0x0003
	.long	panic 
	.long	0x0004
	.long	panic 
	.long	0x0005
	.long	panic 
	.long	0x0006
	.long	panic 
	.long	0x0007
	.long	panic 
	.long	0x0008
	.long	panic 
	.long	0x0009
	.long	panic 
	.long	0x000a		
	.long	panic 
	.long	0x000b
	.long	panic 
	.long	0x000c
	.long	panic 
	.long	0x000d
	.long	panic 
	.long	0x000e
	.long	panic 
	.long	0x000f
	.long	panic 
	.long	0x0010
	.long	panic 
	.long	0x0011
	.long	panic 
	.long	0x0012
	.long	panic  
	.long	0x0013
	.long	panic 
	.long	0x0014		
	.long	panic 
	.long	0x0015
	.long	panic 
	.long	0x0016
	.long	panic 
	.long	0x0017
	.long	panic 
	.long	0x0018
	.long	panic 
	.long	0x0019
	.long	panic 
	.long	0x001a
	.long	panic 
	.long	0x001b
	.long	panic 
	.long	0x001c
	.long	panic 
	.long	0x001d
	.long	panic 
	.long	0x001e
	.long	panic 
	.long	0x001f
	.long	panic 
	.long	0x0020
	.long	panic 
	.long	0x0021
	.long	panic 
	.long	0x0022
	.long	panic 
	.long	0x0023
	.long	panic 
	.long	0x0024
	.long	panic 
	.long	0x0025
	.long	panic 
	.long	0x0026
	.long	panic 
	.long	0x0027
	.long	panic 
	.long	0x0028		
	.long	panic 
	.long	0x0029
	.long	panic 
	.long	0x002a
	.long	panic 
	.long	0x002b
	.long	panic 
	.long	0x002c
	.long	panic 
	.long	0x002d
	.long	panic 
	.long	0x002e
	.long	panic 
	.long	0x002f
	.long	panic 
	.long	0x0030
	.long	panic 
	.long	0x0031
	.long	panic 
	.long	0x0032		
	.long	panic 
	.long	0x0033
	.long	panic 
	.long	0x0034
	.long	panic 
	.long	0x0035
	.long	panic 
	.long	0x0036
	.long	panic 
	.long	0x0037
	.long	panic 
	.long	0x0038
	.long	panic 
	.long	0x0039
	.long	panic 
	.long	0x003a
	.long	panic 
	.long	0x003b
	.long	panic 
	.long	0x003c		
	.long	panic 
	.long	0x003d
	.long	panic 
	.long	0x003e
	.long	panic 
	.long	0x003f
	.text

panic2:
	movl	a0, sp@-			| Save A0 onto stack
	movl	_disptab+16,a0			| Load address of routine
	movl	_disptab+20,sp@-		| Push vector number onto stack
	jsr	a0@				| Jump to interrupt routine
	addql	#4, sp				| Pop vector number from stack
	movl	sp@+, a0			| Restore A0 from stack
	rte					| End Exception Processing
panic3:
	movl	a0, sp@-
	movl	_disptab+24,a0
	movl	_disptab+28,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic4:
	movl	a0, sp@-
	movl	_disptab+32,a0
	movl	_disptab+36,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic5:
	movl	a0, sp@-
	movl	_disptab+40,a0
	movl	_disptab+44,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic6:
	movl	a0, sp@-
	movl	_disptab+48,a0
	movl	_disptab+52,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic7:
	movl	a0, sp@-
	movl	_disptab+56,a0
	movl	_disptab+60,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic8:
	movl	a0, sp@-
	movl	_disptab+64,a0
	movl	_disptab+68,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic9:
	movl	a0, sp@-
	movl	_disptab+72,a0
	movl	_disptab+76,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic10:
	movl	a0, sp@-
	movl	_disptab+80,a0
	movl	_disptab+84,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic11:
	movl	a0, sp@-
	movl	_disptab+88,a0
	movl	_disptab+92,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic24:
	movl	a0, sp@-
	movl	_disptab+192,a0
	movl	_disptab+196,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic25:
	movl	a0, sp@-
	movl	_disptab+200,a0
	movl	_disptab+204,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic26:
	movl	a0, sp@-
	movl	_disptab+208,a0
	movl	_disptab+212,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic27:
	movl	a0, sp@-
	movl	_disptab+216,a0
	movl	_disptab+220,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic28:
	movl	a0, sp@-
	movl	_disptab+224,a0
	movl	_disptab+228,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic29:
	movl	a0, sp@-
	movl	_disptab+232,a0
	movl	_disptab+236,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic30:
	movl	a0, sp@-
	movl	_disptab+240,a0
	movl	_disptab+244,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic31:
	movl	a0, sp@-
	movl	_disptab+248,a0
	movl	_disptab+252,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic32:
	movl	a0, sp@-
	movl	_disptab+256,a0
	movl	_disptab+260,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic33:
	movl	a0, sp@-
	movl	_disptab+264,a0
	movl	_disptab+268,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic34:
	movl	a0, sp@-
	movl	_disptab+272,a0
	movl	_disptab+276,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic35:
	movl	a0, sp@-
	movl	_disptab+280,a0
	movl	_disptab+284,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic36:
	movl	a0, sp@-
	movl	_disptab+288,a0
	movl	_disptab+292,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic37:
	movl	a0, sp@-
	movl	_disptab+296,a0
	movl	_disptab+300,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic38:
	movl	a0, sp@-
	movl	_disptab+304,a0
	movl	_disptab+308,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic39:
	movl	a0, sp@-
	movl	_disptab+312,a0
	movl	_disptab+316,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic40:
	movl	a0, sp@-
	movl	_disptab+320,a0
	movl	_disptab+324,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic41:
	movl	a0, sp@-
	movl	_disptab+328,a0
	movl	_disptab+332,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic42:
	movl	a0, sp@-
	movl	_disptab+336,a0
	movl	_disptab+340,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic43:
	movl	a0, sp@-
	movl	_disptab+344,a0
	movl	_disptab+348,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic44:
	movl	a0, sp@-
	movl	_disptab+352,a0
	movl	_disptab+356,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic45:
	movl	a0, sp@-
	movl	_disptab+360,a0
	movl	_disptab+364,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic46:
	movl	a0, sp@-
	movl	_disptab+368,a0
	movl	_disptab+372,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
panic47:
	movl	a0, sp@-
	movl	_disptab+376,a0
	movl	_disptab+380,sp@-
	jsr	a0@
	addql	#4, sp
	movl	sp@+, a0
	rte
	.globl	_loinit
_loinit:
|	movl	#panic2, 0x8		| Bus Error
|	movl	#panic3, 0xc		| Address Error
|	movl	#panic4, 0x10		| Illegal Instruction
|	movl	#panic5, 0x14		| Zero Divide	
|	movl	#panic6, 0x18		| CHK Instruction
|	movl	#panic7, 0x1c		| TRAPV Instruction
|	movl	#panic8, 0x20		| Privilege Violation
|	movl	#panic9, 0x24		| Trace
|	movl	#panic10, 0x28		| Line 1010 Emulator
|	movl	#panic11, 0x2c		| Line 1111 Emulator
|	movl	#panic24, 0x60		| Spurious Interrupt
|	movl	#panic25, 0x64		| Level 1 Auto
|	movl	#panic26, 0x68		| Level 2 Auto
|	movl	#panic27, 0x6c		| Level 3 Auto
	movl	#panic28, 0x70		| Level 4 Auto
	movl	#panic29, 0x74		| Level 5 Auto
|	movl	#panic30, 0x78		| Level 6 Auto
|	movl	#panic31, 0x7c		| Level 7 Auto
|	movl	#panic32, 0x80		| Trap 0
|	movl	#panic33, 0x84		| Trap 1
|    	movl	#panic34, 0x88		| Trap 2
|	movl	#panic35, 0x8c		| Trap 3
|	movl	#panic36, 0x90		| Trap 4
|	movl	#panic37, 0x94		| Trap 5
|	movl	#panic38, 0x98		| Trap 6
|	movl	#panic39, 0x9c		| Trap 7
|	movl	#panic40, 0xa0		| Trap 8
|	movl	#panic41, 0xa4		| Trap 9
|	movl	#panic42, 0xa8		| Trap 10
|	movl	#panic43, 0xac		| Trap 11
|	movl	#panic44, 0xb0		| Trap 12
|	movl	#panic45, 0xb4		| Trap 13
|	movl	#panic46, 0xb8		| Trap 14
|	movl	#panic47, 0xbc		| Trap 15
	rts
