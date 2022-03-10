/*
 * header for "core11" files
 */
#define	REGISTERS 8
#define	NREGS	  REGISTERS+1	/* including PS */
#define	COREMAGIC 0477
struct core11 {
	short c_magic;
	short c_size;
	short c_zero1;
	short c_zero2;
	short c_regs[REGISTERS];
	short c_psw;
	short c_zero3;
	short c_zero4;
	short c_zero5;
};
#define	R0	0
#define	R1	1
#define	R2	2
#define	R3	3
#define	R4	4
#define	R5	5
#define	R6	6
#define	R7	7
