#define LSILOADER	"/usr/Xinu/lib/sdl.boot.lsi"

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
#define	LOWBYTE		0xff
#define PACKSIZE	0200
#define LSIPORT		0177560
#define INTDISABLE	0340
#define	LSIPSW		(-1)
#define	LSIPC		7
#define	LSISP		6
#define EOS		((char)0)
#define MAXADDR		0160000

#define LOADSTART	0
#define NOAUTOSTART	(-1)

#define MAXFILES	20

typedef unsigned short lsiword;
