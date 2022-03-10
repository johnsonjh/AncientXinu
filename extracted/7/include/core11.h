/* core11.h */

/* Definitions and constants for LSI 11 core dump files */

#define	REGISTERS 	8		/* number of g.p. registers	*/
#define	NREGS		REGISTERS+1	/* total regs including PS	*/
#define	COREMAGIC	0477		/* unix "magic number" for core	*/
struct	core11 {			/* header of 11 core dump file	*/
	short c_magic;			/* UNIX convention		*/
	short c_size;			/* size of valid info (0=>entire*/
					/*  file contains core dump	*/
	short c_zero1;			/* padding to follow UNIX style	*/
	short c_zero2;			/*    "     "    "     "    "	*/
	short c_regs[REGISTERS];	/* dump of machine registers	*/
	short c_psw;			/* dump of program status reg	*/
	short c_zero3;			/* more padding (as in UNIX)	*/
	short c_zero4;			/*   "     "      "  "   "      */
	short c_zero5;			/*   "     "      "  "   "      */
					/* Core image follows header	*/
};

/* Names used for registers */

#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
