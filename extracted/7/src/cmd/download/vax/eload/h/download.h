/* download.h */

#define DEFFILE	"a.out"

#ifndef TRUE
#define TRUE	1
#define FALSE	0
typedef int	Bool;
#endif
#define	LOWBYTE		0xff
#define	MAXARGS		100		/* max # aux. arguments in db	*/
#define	AOUTFD		0		/* fd of a.out downloader reads	*/
#define	DEVFD		1		/* fd of target machine		*/
#define ETHRETRY	3		/* # of times to retry if !ACK	*/
#define	LIBPATH		"/usr/Xinu/lib/"/* path to download binaries	*/
#define	MAXBIN		100		/* max chars in binary file path*/
#define	ACKTIME		3		/* ack timeout in seconds	*/

/*
 * Global argument structure "A"
 */
struct arglist {
	char *filename;
	char *class;
	char *downloader;
	int  machnum;
	int  devflags;
	int  startdelay;
	int  baudrate;
	Bool reloadether;
	Bool verbose;
	Bool silent;
	Bool autostart;
};
