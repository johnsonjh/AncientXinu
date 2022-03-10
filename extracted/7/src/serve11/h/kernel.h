/* kernel.h - disable, enable, halt, restore, isodd, min */

/* Symbolic constants used throughout Xinu */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	NULL		(char *)0	/* Null pointer for linked lists*/
#define	NULLCH		'\0'		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/
#define	SYSCALL		int		/* System call declaration	*/
#define	LOCAL		static		/* Local procedure declaration	*/
#define	COMMAND		int		/* Shell command declaration	*/
#define	BUILTIN		int		/* Shell builtin " "		*/
#define	INTPROC		int		/* Interrupt procedure  "	*/
#define	PROCESS		int		/* Process declaration		*/
#define	RESCHYES	1		/* tell	ready to reschedule	*/
#define	RESCHNO		0		/* tell	ready not to resch.	*/
#define	MININT		0100000		/* minimum integer (-32768)	*/
#define	MAXINT		0077777		/* maximum integer		*/
#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	HIBYTE		0177400		/* mask for high 8 of 16 bits	*/
#define	LOW16		0177777		/* mask for low-order 16 bits	*/
#define	SP		6		/* reg.	6 is stack pointer	*/
#define	PC		7		/* reg.	7 is program counter	*/
#define	PS		8		/* proc. status	in 8th reg. loc	*/
#define	MINSTK		40		/* minimum process stack size	*/
#define	NULLSTK		300		/* process 0 stack size		*/
#define	DISABLE		0340		/* PS to disable interrupts	*/
#define	MAGIC		0125252		/* unusual value for top of stk	*/

/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file (usu. from read)	*/
#define	TIMEOUT		-3		/* time out  (usu. recvtim)	*/
#define	INTRMSG		-4		/* keyboard "intr" key pressed	*/
					/*  (usu. defined as ^B)	*/
/* Initialization constants */

#define	INITSTK		500		/* initial process stack	*/
#define	INITPRIO	20		/* initial process priority	*/
#define	INITNAME	"main"		/* initial process name		*/
#define	INITARGS	1,0		/* initial count/arguments	*/
#define	INITRET		userret		/* processes return address	*/
#define	INITPS		0		/* initial process PS		*/
#define	INITREG		0		/* initial register contents	*/
#define	QUANTUM		10		/* clock ticks until preemption	*/

/* Miscellaneous utility inline functions */

#define	isodd(x)	(01&(int)(x))
#define	min(a,b)	( (a) < (b) ? (a) : (b) )
#define	disable(ps)	asm("mfps ~ps");asm("mtps $0340")
#define	restore(ps)	asm("mtps ~ps")	/* restore interrupt status	*/
#define	enable()	asm("mtps $000")/* enable interrupts		*/
#define	pause()		asm("wait")	/* machine "wait for interr."	*/
#define	halt()		asm("halt")	/* machine halt	instruction	*/

extern	int	rdyhead, rdytail;
extern	int	preempt;
