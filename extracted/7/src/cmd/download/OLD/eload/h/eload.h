/* eload.h - disable, enable, halt, restore, isodd */

/* Symbolic constants used throughout */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	NULL		(char *)0	/* Null pointer for linked lists*/
#define	NULLCH		(char)0		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/
#define	MININT		0100000		/* minimum integer (-32768)	*/
#define	MAXINT		0077777		/* maximum integer		*/
#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	HIBYTE		0177400		/* mask for high 8 of 16 bits	*/
#define	LOW16		0177777		/* mask for low-order 16 bits	*/
#define	SP		6		/* reg.	6 is stack pointer	*/
#define	PC		7		/* reg.	7 is program counter	*/
#define	PS		8		/* proc. status	in 8th reg. loc	*/
#define	DISABLE		0340		/* PS to disable interrupts	*/

/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file			*/
#define	TIMEOUT		-3		/* time out			*/

/* Miscellaneous utility inline functions */

#define	isodd(x)	(01&(int)(x))
#define	disable(ps)	asm("mfps ~ps");asm("mtps $0340")
#define	restore(ps)	asm("mtps ~ps")	/* restore interrupt status	*/
#define	enable()	asm("mtps $000")/* enable interrupts		*/
#define	halt()		asm("halt")	/* machine halt	instruction	*/

#define	CDEVICE		0177560		/* Console device address	*/
#define	EDEVICE		0174440		/* DEQNA device address		*/
#define	ETHINTV		0200		/* DEQNA interrupt vector	*/
#define	ACK		'A'		/* Protocol positive Acknowledg.*/
#define	NACK		'N'		/* Protocol neg. acknowledgment	*/
#define	QACK		'Q'		/* Protocol "am quiting" ack.	*/

#define kputc(x,c)	while((x->ctstat&SLUREADY)==0) ; x->ctbuf=c
