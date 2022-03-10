/* qbus.h */

						/* UVAXII addrs of regs.:   */
#define QMAP	((unsigned int *)0x20088000)	/*   qbus map regs.         */
#define QMMECSR	((unsigned int *)0x20080004)	/*   mem error control/stat */
#define	QMIPCR	((unsigned short *)0x20001f40)	/*   cpu communic. reg      */
#define	QMCPMBX	0x200b801c			/*   console program mailbox*/

						/* UVAXII bus mapping regs: */
#define	QMNMREG	8192				/* # mapping registers      */
#define	QMENAB	0x20				/* enable bus mapping       */
#define	QMREGV	0x80000000			/* map register valid	    */
#define	QMHALT	0x23				/* QMCPMBX value to halt cpu*/

#ifndef	ASM

#define	LOW6	077
	/* extract high 6 bits of 22-bit Q-bus address into low 6 bits*/
#define	hiQ2low(x) (short)(((unsigned int)x>>(sizeof(short)*8)) & LOW6)

	/* move low 6 bits from short to high 6 bits of 22-bit Qbus address */
#define	lowQ2hi(x) (((int)x & LOW6) << (sizeof(short)*8))

#endif	ASM
