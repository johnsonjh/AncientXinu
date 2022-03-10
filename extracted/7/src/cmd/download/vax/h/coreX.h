/* coreX.h */

/* Definitions and constants for VAX Xinu core dump files */

#define	REGISTERS 	24		/* number of g.p. registers	*/
#define	COREMAGIC	0477		/* unix "magic number" for core	*/
struct	coreX {				/* header of VAX core dump file	*/
	short c_magic;			/* UNIX convention		*/
	int   c_size;			/* size of valid info (0=>entire*/
					/*  file contains core dump	*/
	short c_zero1;			/* padding to follow UNIX style	*/
	short c_zero2;			/*    "     "    "     "    "	*/
	int   c_regs[REGISTERS];	/* dump of machine registers	*/
	int   c_psw;			/* dump of program status reg	*/
	short c_zero3;			/* more padding (as in UNIX)	*/
	short c_zero4;			/*   "     "      "  "   "      */
	short c_zero5;			/*   "     "      "  "   "      */
					/* Core image follows header	*/
};

#define	KSP	0
#define	ESP	1
#define	SSP	2
#define	USP	3
#define	R0	4
#define	R1	5
#define	R2	6
#define	R3	7
#define	R4	8
#define	R5	9
#define	R6	10
#define	R7	11
#define	R8	12
#define	R9	13
#define	R10	14
#define	R11	15
#define	AP	16
#define	FP	17
#define	PC	18
#define	PSL	19
#define	P0BR	20
#define	P0LR	21
#define	P1BR	22
#define	P1LR	23
