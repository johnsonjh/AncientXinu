/* upload.h -- definitions for generic uploader */

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
typedef	int		Bool;
#endif
#define	EOS		((char)0)
#define	SOH		'/'
#define	ESC		'\033'
#define	COREFD		0		/* fd of output core file	*/
#define	DEVFD		1		/* fd of machine's console line	*/
#define	LOWBYTE		0xff
#define	MAXARGS		100		/* maximum # aux. arguments in db   */
#define ETHRETRY	3		/* number of times to retry if !ACK */
#define	LIBPATH		"/usr/Xinu/lib/"/* path to upload binaries	    */
#define	MAXBIN		100		/* max chars in binary file path    */

#define settimer(t,m)	msgval = m; alarm(t)
#define canceltimer()	alarm(0)
#define	unctrl(ch)	(_unctrl[ch])

char	*_unctrl[] = {			/* unctrl codes	for ttys	*/
	"^@","^A","^B","^C","^D","^E","^F","^G","^H","^I","^J","^K",
	"^L","^M","^N","^O","^P","^Q","^R","^S","^T","^U","^V","^W",
	"^X","^Y","^Z","^[","^\\","^]","^~","^_",
	" ","!","\"","#","$", "%","&","'","(",")","*","+",",","-",
	".","/","0", "1","2", "3","4","5","6","7","8","9",":",";",
	"<","=",">", "?","@", "A","B","C","D","E","F","G","H","I",
	"J","K","L", "M","N", "O","P","Q","R","S","T","U","V","W",
	"X","Y","Z", "[","\\","]","^","_","`","a","b","c","d","e",
	"f","g","h", "i","j", "k","l","m","n","o","p","q","r","s",
	"t","u","v", "w","x", "y","z","{","|","}","~","^?"
};

struct	arglist {
	char	*uploader;
	char	*bootloader;
	char	*class;
	int	machnum;
	int	baudrate;
	int	devflags;
	char	*corefile;
	int	lowaddr;
	int	highaddr;
	Bool	verbose;
	Bool	silent;
	Bool	noload;
};
