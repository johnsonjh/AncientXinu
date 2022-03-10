/* lowcore.s - (map of interupt vectors in low part of memory) */

DISABLE	=	340			/ PS to	disable	interrupts
ENABLE	=	000			/ PS to	enable interrupts

/*------------------------------------------------------------------------
/*  absolute location 0  -- fixed interrupt and exception vectors
/*------------------------------------------------------------------------
.	=	000^.			/ panic because something jumped
	jmp	panic0			/     to location zero
.	=	004^.			/ bus error (e.g., malfunction)
	panic;	DISABLE+0
.	=	010^.			/ illegal/reserved instruction
	panic;	DISABLE+1
.	=	014^.			/ BPT instruction and T bit
	panic;	DISABLE+2
.	=	020^.			/ Input/Output trap
	panic;	DISABLE+3
.	=	024^.			/ Power	fail
	panic;	DISABLE+4
.	=	030^.			/ EMT instruction, emulator trap
	panic;	DISABLE+5
.	=	034^.			/ TRAP instruction
	panic;	DISABLE+6
.	=	040^.
	panic;	DISABLE+7
.	=	044^.
	panic;	DISABLE+7
.	=	050^.
	panic;	DISABLE+7
.	=	054^.
	panic;	DISABLE+7
.	=	060^.			/ Console terminal, input
	panic;	DISABLE+7		/  (usually, device 0)
.	=	064^.			/ Console terminal, output
	panic;	DISABLE+7		/  (usually, device 0)
.	=	070^.
	panic;	DISABLE+7
.	=	074^.
	panic;	DISABLE+7
.	=	100^.			/ BEVENT (real-time clock)
	panic;	DISABLE+7
.	=	104^.
	panic;	DISABLE+7
.	=	110^.
	panic;	DISABLE+7
.	=	114^.
	panic;	DISABLE+7
.	=	120^.
	panic;	DISABLE+7
.	=	124^.			/ DRV11-B
	panic;	DISABLE+7
.	=	130^.
	panic;	DISABLE+7
.	=	134^.
	panic;	DISABLE+7
.	=	140^.
	panic;	DISABLE+7
.	=	144^.
	panic;	DISABLE+7
.	=	150^.
	panic;	DISABLE+7
.	=	154^.
	panic;	DISABLE+7
.	=	160^.			/ RLV11	DMA interface
	panic;	DISABLE+7
.	=	164^.
	panic;	DISABLE+7
.	=	170^.
	panic;	DISABLE+7
.	=	174^.
	panic;	DISABLE+7
.	=	200^.			/ LAV11, LPV11 line printers
	panic;	DISABLE+7
.	=	204^.
	panic;	DISABLE+7
.	=	210^.
	panic;	DISABLE+7
.	=	214^.
	panic;	DISABLE+7
.	=	220^.			/ RKV11	disk
	panic;	DISABLE+7
.	=	224^.
	panic;	DISABLE+7
.	=	230^.
	panic;	DISABLE+7
.	=	234^.
	panic;	DISABLE+7
.	=	240^.
	panic;	DISABLE+7
.	=	244^.			/ Floating point error
	panic;	DISABLE+7
.	=	250^.			/ Memory management (11/23)
	panic;	DISABLE+7
.	=	254^.
	panic;	DISABLE+7
.	=	260^.
	panic;	DISABLE+7
.	=	264^.			/ RXV11, RXV21 floppy disks
	panic;	DISABLE+7
.	=	270^.
	panic;	DISABLE+7
.	=	274^.
	panic;	DISABLE+7
.	=	300^.			/ Start	of floating vectors
	panic;	DISABLE+7
.	=	304^.
	panic;	DISABLE+7
.	=	310^.
	panic;	DISABLE+7
.	=	314^.
	panic;	DISABLE+7
.	=	320^.
	panic;	DISABLE+7
.	=	324^.
	panic;	DISABLE+7
.	=	330^.
	panic;	DISABLE+7
.	=	334^.
	panic;	DISABLE+7
.	=	340^.
	panic;	DISABLE+7
.	=	344^.
	panic;	DISABLE+7
.	=	350^.
	panic;	DISABLE+7
.	=	354^.
	panic;	DISABLE+7
.	=	360^.
	panic;	DISABLE+7
.	=	364^.
	panic;	DISABLE+7
.	=	370^.
	panic;	DISABLE+7
.	=	374^.
	panic;	DISABLE+7
.	=	400^.
	panic;	DISABLE+7
.	=	404^.
	panic;	DISABLE+7
.	=	410^.
	panic;	DISABLE+7
.	=	414^.
	panic;	DISABLE+7
.	=	420^.
	panic;	DISABLE+7
.	=	424^.
	panic;	DISABLE+7
.	=	430^.
	panic;	DISABLE+7
.	=	434^.
	panic;	DISABLE+7
.	=	440^.
	panic;	DISABLE+7
.	=	444^.
	panic;	DISABLE+7
.	=	450^.
	panic;	DISABLE+7
.	=	454^.
	panic;	DISABLE+7
.	=	460^.
	panic;	DISABLE+7
.	=	464^.
	panic;	DISABLE+7
.	=	470^.
	panic;	DISABLE+7
.	=	474^.
	panic;	DISABLE+7
.	=	500^.
	panic;	DISABLE+7
.	=	504^.
	panic;	DISABLE+7
.	=	510^.
	panic;	DISABLE+7
.	=	514^.
	panic;	DISABLE+7
.	=	520^.
	panic;	DISABLE+7
.	=	524^.
	panic;	DISABLE+7
.	=	530^.
	panic;	DISABLE+7
.	=	534^.
	panic;	DISABLE+7
.	=	540^.
	panic;	DISABLE+7
.	=	544^.
	panic;	DISABLE+7
.	=	550^.
	panic;	DISABLE+7
.	=	554^.
	panic;	DISABLE+7
.	=	560^.
	panic;	DISABLE+7
.	=	564^.
	panic;	DISABLE+7
.	=	570^.
	panic;	DISABLE+7
.	=	574^.
	panic;	DISABLE+7
.	=	600^.
	panic;	DISABLE+7
.	=	604^.
	panic;	DISABLE+7
.	=	610^.
	panic;	DISABLE+7
.	=	614^.
	panic;	DISABLE+7
.	=	620^.
	panic;	DISABLE+7
.	=	624^.
	panic;	DISABLE+7
.	=	630^.
	panic;	DISABLE+7
.	=	634^.
	panic;	DISABLE+7
.	=	640^.
	panic;	DISABLE+7
.	=	644^.
	panic;	DISABLE+7
.	=	650^.
	panic;	DISABLE+7
.	=	654^.
	panic;	DISABLE+7
.	=	660^.
	panic;	DISABLE+7
.	=	664^.
	panic;	DISABLE+7
.	=	670^.
	panic;	DISABLE+7
.	=	674^.
	panic;	DISABLE+7
.	=	700^.
	panic;	DISABLE+7
.	=	704^.
	panic;	DISABLE+7
.	=	710^.
	panic;	DISABLE+7
.	=	714^.
	panic;	DISABLE+7
.	=	720^.
	panic;	DISABLE+7
.	=	724^.
	panic;	DISABLE+7
.	=	730^.
	panic;	DISABLE+7
.	=	734^.
	panic;	DISABLE+7
.	=	740^.
	panic;	DISABLE+7
.	=	744^.
	panic;	DISABLE+7
.	=	750^.
	panic;	DISABLE+7
.	=	754^.
	panic;	DISABLE+7
.	=	760^.
	panic;	DISABLE+7
.	=	764^.
	panic;	DISABLE+7
.	=	770^.
	panic;	DISABLE+7
.	=	774^.
	panic;	DISABLE+7
