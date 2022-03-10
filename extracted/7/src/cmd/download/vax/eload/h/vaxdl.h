/* vaxdl.h */

#define VAXLOADER	"/usr/Xinu/lib/sdl.boot.vax"

#ifndef	ASM

#define settimer(t,m)	alarm_msg = m; alarm(t)
#define canceltimer()	alarm(0)
#define	unctrl(ch)	((ch > 128 || ch < 0) ? "<non ASCII>" : (_unctrl[ch]))

char	*_unctrl[]	= {	/* unctrl codes for ttys		*/
	"^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H", "^I", "^J",
	"^K", "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T", "^U",
	"^V", "^W", "^X", "^Y", "^Z", "^[", "^\\", "^]", "^~", "^_", " ",
	"!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-",
	".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":",
	";", "<", "=", ">", "?", "@",  "A", "B", "C", "D", "E", "F", "G",
	"H", "I", "J", "K", "L", "M", "N",  "O", "P", "Q", "R", "S", "T",
	"U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`", "a",
	"b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
	"p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}",
	"~", "^?"
};

#define SOH		'/'
#define ACK		'y'
#define NAK		'n'
#define ESC		'\033'
#define	LOCAL		static
#define	SOHSTR		"/"

#else	ASM

#define	SOH		0x2f		/* '/'				*/
#define	ACK		0x79		/* 'y'				*/
#define	NAK		0x6e		/* 'n'				*/
#define	ESC		0x1b

#endif	ifndef	ASM

#define	LOWBYTE		0xff
#define PACKSIZE	0200
#define EOS		((char)0)
#define LOADSTART	0
#define NOAUTOSTART	(-1)
#define MAXFILES	20
#define	LOW16		0xffff
#define	CVECTOR		0xc0		/* clock's interrupt vector	*/
#define	SYSTYPE		0x00ffffff	/* mask for system id register	*/
#define	VAX8600SID	0x04000000	/* SID register value for 8600	*/
#define	UVAXIISID	0x08000000	/* SID register value for UVAXII*/
#define	RXCS8600	0x10000		/* setup values for 8600 console*/
#define	TXCS8600	0x18000		/* trans intrpts not enabled	*/
#define	CLKINTVL	10000		/* usec countdown for intvl clk	*/
#define	CLKIPL		0x15		/* intvl clk IPL (low for UVAX)	*/
#define	CLEARINT	0xc1		/* clear clock ICCS INT bit	*/
#define	STRTCLK		0x51		/* value to start VAX timer	*/
#define	STUFFBIT	0100		/* used for stuffing past ^P	*/
#define	CNSINTRC	'\020'		/* ^P				*/
#define	CNSINTRC2	'\220'		/* ^P with parity bit set	*/
#define	CONSKILL	"\025"		/* ^U				*/
#define	CONSABRT	"\003"		/* ^C				*/
#define	CONSINTR	"\020"		/* ^P				*/
#define	CPUHALT		"H\r"		/* halts the cpu		*/
#define	CPUNJAM		"U\r"		/* unjams the bus		*/
#define	CPUINIT		"I\r"		/* initialize the cpu		*/
#define	KSPREG		-1001		/* " " for kernel stack ptr	*/
#define	SPREG		0xe		/* stack pointer register	*/
#define	PCREG		0xf		/* PC general register		*/
#define	P0BSTRT		0x400		/* load addr. phase0 bootstrap	*/
#define	P0MARKER	0xffffffff	/* marks end of phase0 bootstrap*/
#define	NVECBYTE	0x400		/* # of VAX intrrpt vector bytes*/
#define HIADDR		(0x100000 - (PACKSIZE*2) - 8192)
	/* addr. last byte of phase1+1 (assume at least 1 meg, but	*/
	/* leave space if UVAXII for console database, space for packet)*/
	/* HIADDR must be a multiple of 4				*/
