/* regdefs.h - setareg, setdreg, setpc, stps, setusp */

#define ADDRESS	'A'
#define DATA	'D'
#define	SPECIAL	'S'
#define MEMORYWORD 'W'
#define MEMORYBYTE 'B'

#define	SS	1
#define	US	2
#define	SR	3
#define	PC	4

#define	setareg(reg, value)	setreg(reg, value, ADDRESS)
#define	setdreg(reg, value)	setreg(reg, value, DATA)
#define setpc(value)		setreg(PC, value, SPECIAL)
#define setps(value)		setreg(SR, value, SPECIAL)
#define setusp(value)		setreg(US, value, SPECIAL)
#define setbyte(loc, value)	setreg(loc, value, MEMORYBYTE)
#define setword(loc, value)	setreg(loc, value, MEMORYWORD)
#define setlong(loc, value)	setword(loc, (value >> 16) & 0xffff);	\
				setword(loc+2, (value) & 0xffff);
