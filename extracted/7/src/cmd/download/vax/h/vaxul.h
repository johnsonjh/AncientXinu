/* vaxul.h -- definitions specific to VAX uploader */

#define	DEFFILE		"coreX"
#define	ACKTIME		3
#define	MAXADDR		0x20000000
#define NOAUTOSTART	(-1)
#define	LOW16		0xffff
#define	CONSKILL	"\025"		/* ^U				*/
#define	CONSABRT	"\003"		/* ^C				*/
#define	CONSINTR	"\020"		/* ^P				*/
#define	CPUHALT		"H\r"		/* halts the cpu		*/
#define	CPUNJAM		"U\r"		/* unjams the bus		*/
#define	CPUINIT		"I\r"		/* initialize the cpu		*/
#define	KSPREG		-1001		/* " " for kernel stack ptr	*/
#define	SPREG		0xe		/* stack pointer register	*/
#define	PCREG		0xf		/* PC general register		*/
