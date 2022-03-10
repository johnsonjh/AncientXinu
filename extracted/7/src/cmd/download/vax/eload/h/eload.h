/* eload.h - disable, enable, halt, restore, isodd */

/* Symbolic constants used throughout */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	NULL		(char *)0	/* Null pointer for linked lists*/
#define	NULLCH		(char)0		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/
#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	LOW16		0177777		/* mask for low-order 16 bits	*/

/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
/* Miscellaneous utility inline functions */

#define	isodd(x)	(01&(int)(x))
#define	EDEVICE		0x20001920	/* DEQNA csr addr (UVAX I or II)*/
#define	ETHINTV		0200		/* DEQNA interrupt vector	*/
