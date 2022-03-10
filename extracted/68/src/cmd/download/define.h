#include <stdio.h>

#define	BOOTLOADER	"/v1/burns/Xinu68/src/cmd/download/boot.68"
#define	FASTLOADER	"/v1/burns/Xinu68/src/cmd/download/fast.o"
#define LOCKFILE 	"/tmp/SUNlock"
#define LOGFILE		"/v1/burns/Xinu68/src/cmd/download/.logfile"
#define USERPROG	"a.out"
#define TRUE		1
#define FALSE		0
#define	WAIT		TRUE
#define	NOWAIT		FALSE
#define SOH		'/'
#define ACK		'y'
#define NAK		'n'
#define ESC		'\033'
#define PACKSIZE	0x0400
#define DISABLE		0x2700
#define EOS		'\0'
#define MAXADDR		0x40000
#define BOOTSTART	0x1000
#define PROGSTART	0x1000
#define unctrl(ch)	(_unctrl[ch])

typedef	int	Bool;

struct options {
	char	*fastloader;
	char	*bootloader;
	char	*userprog;
	char	*logfile;
	int	startdelay;
	int	loadaddr;
	Bool	verbose;
	Bool	silent;
	Bool	autostart;
};

extern	char	*_unctrl[];
extern	FILE	*logfp;
extern	struct 	options	A;
extern	int	ttyset;
