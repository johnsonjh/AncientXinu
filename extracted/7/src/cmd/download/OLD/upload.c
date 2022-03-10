/*----------------------------------------------------------------------*/
/*									*/
/*	upload        +++ copy LSI-11 memory to ./core11 +++		*/
/*									*/
/* This program takes LSI-11 memory and places it into a file named	*/
/* core11 on the host system						*/
/*									*/
/* Configuration -							*/
/*	VAX tty line connected to LSI 11 ODT (console) port		*/
/*									*/
/* Usage:								*/
/*	upload [filename] [-lx] [-o filename]				*/
/*									*/
/* Authors:								*/
/*	R. Brown & C. Kent						*/
/*	Purdue University/CS Deptartment				*/
/*	September, 1981							*/
/*	mods by Comer, 11/81-11/83					*/
/*									*/
/*----------------------------------------------------------------------*/

#include <stdio.h>
#include <sgtty.h>
#include <signal.h>
#include <errno.h>
#include "/usr/Xinu/include/core11.h"
#include "/usr/Xinu/include/a.out.h"
#include "/usr/Xinu/include/baud.h"
#include "getdev.h"

#define	MAXADDR		0157777
#define	CSRBASE		0177560
#define	LOADSTART	0
typedef	unsigned short	lsiword;

#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
typedef	int		Bool;
#endif
#define	EOS		((char)0)
#define	SOH		'/'
#define	ESC		'\033'

#define	LOADER		"/usr/Xinu/lib/ul"
#define	MAXPACKSIZE	526
#define	STACKLEN	20

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

struct	packet	{
	lsiword	len, base;
	lsiword	buf[MAXPACKSIZE];
	lsiword	sum;
};

struct {
	char	*odtline;
	int	baudrate;
	char	*corefile;
	char	*loader;
	int	lowaddr;
	int	highaddr;
	Bool	verbose;
	Bool	silent;
	Bool	noload;
	char	*outfile;
} A;

char 	*msgval;
char	*Openfiles[20];
struct	core11	Corehead;
int	Devfd,	Corefd;
int	Haltedat;
struct	sgttyb	VAXttyb;
lsiword *Memory;
char	*index();
char	class[MAXNAMELEN];
int	useclass;
int	machnum;
char	dev[MAXNAMELEN], devname[MAXNAMELEN];
char	lockfile[40];
int	touch(), getdev();
extern	int	errno;
int	use;

/*----------------------------------------------------------------------
 *  upload  -- upload an LSI 11 memory image
 *----------------------------------------------------------------------
 */
main(argc,argv)
int	argc;
char	*argv[];
{
	int alarmhandler(), inthandler();
	int ldrfd, bytes;
	int base,length,i;
	char buf[32];
	struct exec bhdr;
	short entry;

	/*
	 * set up terminal modes...prepare to reset on interrupt
	 */
	procargs(argc, argv);
	if ((machnum = getdev(useclass,class,machnum,&Devfd,lockfile)) < 0)
		exit(1);
	if (machnum == 0) {
		strcpy(devname, class);
		if (! A.silent)
			printf("Using %s\n", class);
	}
	else {
		sprintf(devname, DEVNAME, DEVDIR, class);
		if (! A.silent)
			printf("Using %s #%d\n", class, machnum);
	}
	Openfiles[Devfd] = sprintf(dev, devname, machnum);
	Corefd = creat(A.corefile,0666);
	ldrfd =	openfile(A.loader,0);
	gtty(Devfd,&VAXttyb);
	/*
	 * trap	software signals so that lines can be restored to their
	 * original state and the core file can	be written
	 */
	if ( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal(SIGINT,inthandler);
	if ( signal(SIGTERM,SIG_IGN) !=	SIG_IGN	)
		signal(SIGTERM,inthandler);
	if ( signal(SIGQUIT,SIG_IGN) !=	SIG_IGN	)
		signal(SIGQUIT,inthandler);
	linemode(Devfd,A.baudrate,RAW); /*|TANDEM*/

	signal(SIGALRM,	alarmhandler);
	/*
	 * allocate memory array - this	written	to the core file when the
	 * program terminates
	 */
	if (A.verbose)
		printf("Alocating %d (0%o) bytes for image\n",MAXADDR+1,
			MAXADDR+1);
	Memory = (lsiword *)malloc(MAXADDR+1);
	for ( i=0 ; i<(MAXADDR+1)/2 ; i++ )
		Memory[i] = 0;
	Corehead.c_zero1 = 0;
	Corehead.c_zero2 = 0;
	Corehead.c_zero3 = 0;
	Corehead.c_zero4 = 0;
	Corehead.c_zero5 = 0;
	Corehead.c_magic = COREMAGIC;

        message("Getting ODT...",0,0);
	getodt(Devfd);
	/*
	 * Get the contents of the registers
	 */
	message("Getting registers...",0,0);
	getregs(Devfd,Corehead.c_regs);
	getpsw(Devfd,&Corehead.c_psw);
	if (A.verbose)
		message("Getting a.out header",0,0);
	gethdr(ldrfd,&bhdr);
	bytes =	bhdr.a_text+bhdr.a_data+STACKLEN;
	if (A.verbose)
		message("Number of bytes = %d",bytes,0);
	if ( !A.noload ) {
		message("Deposit boot loader...",0,0);
		entry =	odtload(ldrfd,&bhdr,0,Memory);
	} else {
		message("Not loading boot loader...",0,0);
		read ( ldrfd, Memory, bytes*sizeof(lsiword) );
	}
	/*
	 * Set up arguments to upl and start it	running...
	 */
	if (A.verbose)
		message("Getting ODT again...",0,0);
	getodt(Devfd);
	message("Starting load of 0%o-0%o...",A.lowaddr,A.highaddr);
	base = (A.lowaddr < bytes ? bytes :	A.lowaddr);
	length = (A.highaddr-base + 1)/2;
	if (A.verbose)
		message("Setting argument registers",0,0);
	setreg(Devfd,0,base);
	setreg(Devfd,1,length);
	setreg(Devfd,4,CSRBASE);
	setreg(Devfd,6,base);
	if (A.verbose)
		message("Starting LSI 11 execution",0,0);
	sendodt(Devfd,"P",TRUE);
	fastread(Devfd,Memory);
	message("Writing core file...",0,0);
	write(Corefd,&Corehead,sizeof Corehead);
	write(Corefd,Memory,A.highaddr+1);
	touch(lockfile);
	message("Done",0,0);

}
/*
 *===================================================
 * procargs - process argument in global structure
 *===================================================
 *
 * This	procedure continas the logic for converting the	UNIX argument
 * list	into global variables
 */
procargs(argc, argv)
int argc;
char *argv[];
{
	int arg, i, unswitched, more;
	int  afd;
	struct exec ahdr;
	char *swptr;
	/*
	 * assign default values
	 */
	A.lowaddr = 0;
	A.highaddr = MAXADDR;
	A.corefile = "core11";
	A.odtline = NULL;
	A.baudrate = B9600;
	strcpy(class, "LSI");
	useclass = FALSE;
	machnum = MYDEV;
	A.loader = LOADER;
	A.verbose = FALSE;
	A.silent = FALSE;
	A.noload = FALSE;
	unswitched = 0;
	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			more = TRUE;
			swptr =	&argv[arg][1];
			while (	more &&	*swptr!='\0' ) {
				switch ( *swptr	) {
				case 'o':
					if ( arg+1 >= argc )
						usagexit(argv[0]);
					A.corefile = argv[arg++];
					more = FALSE;
					break;
				case 'v':
					A.verbose = TRUE;
					break;
				case 's':
					A.silent = TRUE;
					break;
				case 'n':
					A.noload = TRUE;
					break;
				case 'a':
					afd = openfile(argv[arg++],0);
					if ( arg+1 >= argc )
						usagexit(argv[0]);
					gethdr(afd,&ahdr);
					A.lowaddr = ahdr.a_text;
					A.highaddr = ahdr.a_text+ahdr.a_data+
							ahdr.a_bss;
					more = FALSE;
					break;
				case 'c':
					argv[arg] += 2;
					strcpy(class, argv[arg]);
					useclass = TRUE;
					more = FALSE;
					break;
				case 'l':
					argv[arg] += 2;
					if ( (machnum = atoi(argv[arg])) < 0)
						usagexit(argv[0]);
					more = FALSE;
					break;
				case 't':
					argv[arg] += 2;
					strcpy(class, argv[arg]);
					machnum = TTYNAME;
					more = FALSE;
					break;
				case 'B':
					if (*(argv[arg]+=2) == '\0') {
						A.baudrate = NOBAUD;
						break;
					}
					for (i = 0; i < NBAUD; ++i)
						if (strcmp(baudlist[i].name,
							argv[arg]) == 0)
								break;
					if (i < NBAUD) {
					printf("baud = %s (%d)\n", baudlist[i].name, baudlist[i].rate);
						A.baudrate = baudlist[i].rate;
					}
					else {
						fprintf(stderr,
						    "Unknown baud rate: %s\n",
							argv[arg]);
						exit(1);
					}
					more = FALSE;
					break;
				default:
					usagexit(argv[0]);
				}
				swptr++;
			}
		} else { /* there's no dash in front */
			switch ( unswitched++ )	{
			case 0:
				A.highaddr = atoi(argv[arg]);
				message("highaddr=%o",A.highaddr,0);
				break;
			case 1:
				A.lowaddr = A.highaddr;
				A.highaddr = atoi(argv[arg]);
				message("lowaddr=%o",A.lowaddr,0);
				message("highaddr=%o",A.highaddr,0);
				break;
			default:
				usagexit(argv[0]);
			}
		}
	}
}
usagexit(pgm)
char *pgm;
{
	fprintf(stderr,"usage: %s [-options] [[lowaddr] highaddr]\n",pgm);
	exit(1);
}
/*
 *==========================================================
 * openfile - try to open file,	exit with message if error
 *==========================================================
 */
openfile(name,mode)
char *name;
int mode;
{
	int fd;
	settimer(10, "open timed out");
	if ((fd=open(name,mode)) < 0 ) {
		perror(name);
		exit(1);
	}
	canceltimer();
	Openfiles[fd] =	name;
	return(fd);
}
/*
 *=========================================
 * alarmhandler	- return from alarm calls
 *=========================================
 */
alarmhandler()
{
	signal(SIGALRM,	alarmhandler);
	fprintf(stdout, "%s\n", msgval);
	exit(1);
}

/*
 *=====================================
 * inthandler -- interrupt processing
 *=====================================
 */
inthandler()
{
	write(Corefd,&Corehead,sizeof Corehead);
	write(Corefd,Memory,A.highaddr+1);
	touch(lockfile);
	callexit(2);
}
/*
 *=======================================
 * callexit - restore ttymodes and exit
 *=======================================
 */
callexit(ret)
int ret;
{
	/* stty(Devfd,&VAXttyb); */
	exit(ret);
}

/*
 *==========================================
 * odtload - load bootstrap loader via ODT
 *==========================================
 * input state:	after ODT prompt
 * output state: same
 *
 * fdin	- a.out	file to	download
 * hdr - a.out header for file to load
 * loc - location to begin loading (overrides default 0)
 * outaddr - ptr to int	array to hold current contents,	or NULL
 */
odtload(fdin, hdr, loc,	outaddr)
int fdin;
int loc;
lsiword	*outaddr;
struct exec *hdr;
{
	int addr, lsival;
	int i, length;
	lsiword	word;
	char buf[32], filebuf[BUFSIZ],c,*val;
	FILE *infile;

	infile = fdopen(dup(fdin),"r");
	fseek(infile,sizeof *hdr,0);
	setbuf(stdout,NULL);
	setbuf(infile,filebuf);
	length = hdr->a_text+hdr->a_data+STACKLEN;
	addr = LOADSTART;
	do {
		fread(&word, sizeof word, 1, infile);
		if ( addr == LOADSTART ) {
			sprintf(buf,"%o/",addr+loc);
			sendodt(Devfd,buf,TRUE);
			readuntil(Devfd, " ?", buf, 7);
			val = buf;
		} else {
			sendodt(Devfd, "\n",TRUE);
			readuntil(Devfd, " ?", buf, 7);
			val = index(buf,'/')+1;
		}
		if ( outaddr !=	NULL ) {
			sscanf(val,"%o ",&lsival);
			outaddr[(addr+loc)/2] =	lsival & 0xffff;
		}
		sprintf(buf, "%o", word	& 0xffff);
		sendodt(Devfd, buf,TRUE);
		if ( !A.silent && !A.verbose )
			displayval(addr);
		addr +=	sizeof (lsiword);
	} while	( word != 0177777 );
	if ( !A.silent && !A.verbose )
		printf("\r");
	sendodt(Devfd,"\r",TRUE);
	readuntil(Devfd, "@", buf, 5);
	/*
	 * now,	run this half to load the other half faster
	 */
	fseek(infile,sizeof(struct exec),0);
	setreg(Devfd,1,length);
	setreg(Devfd,-1,0340);
	setreg(Devfd,7,LOADSTART);
	sendodt(Devfd,"P",FALSE);
        sleep(1);
	for ( i=0 ; i<length ; i++ ){
		c = getc(infile);
		if (A.verbose){
			fprintf(stdout,"AT: %06o ", i&0xffff);
			fprintf(stdout,"OUT: %03o ",c&0xff);
		}
		write(Devfd,&c,1);
		read(Devfd,&c,1);
		if (A.verbose) fprintf(stdout,"IN: %03o\n",c&0xff);
		if ( i>=addr && outaddr != NULL )
			*(((char *)outaddr)+i) = c;
	}
	readuntil(Devfd,"@", buf, 8);
	return(addr);
}
/*
 *===============================================
 * getpsw - get	the current contents of	the PSW
 *===============================================
 */
getpsw(fd,psw)
int fd;
short *psw;
{
	char buf[60];
	char	*reply;

	sendodt(Devfd,"$S/",TRUE);
	readuntil(Devfd," ?", buf, 5);
	reply = index(buf,'/');
	sscanf( ((0==(int)reply) ? buf : reply+1),"%o ",psw);
	sendodt(Devfd,"\r",TRUE);
	readuntil(Devfd,"@",buf,5);
}
/*
 *==========================================
 * getregs - get contents of the registers
 *==========================================
 */
getregs(fd,regs)
int fd;
short *regs;
{
	int r;
	int lsival;
	char buf[32], *val;
	for ( r=0 ; r<REGISTERS	; r++ )	{
		if ( r==0 ) {
			sprintf(buf,"R%d/",r);
			sendodt(Devfd,buf,TRUE);
			readuntil(Devfd, " ?", buf, 7);
			val = buf;
		} else {
			sendodt(Devfd, "\n",TRUE);
			readuntil(Devfd, " ?", buf, 7);
			val = index(buf,'/')+1;
		}
		sscanf(val,"%o ",&lsival);
		regs[r]	= lsival & 0xffff;
		if ( !A.silent && !A.verbose )
			displayval(r);
	}
	if ( !A.silent && !A.verbose )
		printf("\r");
	sendodt(Devfd,"\r",TRUE);
	readuntil(Devfd, "@", buf, 5);
}

/*
 *===============================
 * getodt - get	ODT's attention
 *===============================
 * Input state - undefined
 * Output state	- immediately after ODT	prompt
 */
getodt(fd)
int fd;
{
	char buf[BUFSIZ];

	ioctl(fd, TIOCSBRK, NULL);
	sleep(2);
	ioctl(fd, TIOCCBRK, NULL);
	readuntil(fd,"@",buf,5);
	sscanf(buf,"%o",&Haltedat);
}
/*
 *==================================================
 * readuntil - read from line until some character
 *==================================================
 */
readuntil ( fd,	ch, buf, time )
int fd,	time;
char *ch, *buf;
{
	settimer(time, "read timed out");
	if ( A.verbose )
		printf("IN: ");
	do {
		if ( read(fd, buf, 1) != 1 ) {
			perror(Openfiles[fd]);
			callexit(1);
		}
		if (A.verbose )
			printf("%s",unctrl(*buf));
	} while	( *buf == '\0' || index(ch,*buf++) == NULL );
	*buf = EOS;
	canceltimer();
	if ( A.verbose )
		printf("\n");
}
/*
 *==============================================
 * sendodt - send a message to ODT half duplex
 *==============================================
 */
sendodt(fd, msg, wait)
int fd;
char *msg;
Bool wait;
{

	char buf[32], *ptr, tmpstr[2];
	if ( A.verbose ) {
		printf("OUT: ");
		for ( ptr=msg ; *ptr!=EOS ; ptr++ ) {
			printf("%s",unctrl(*ptr));
		}
		printf("\n");
	}
	while (*msg!=EOS) {
		write(fd,msg,1);
                if ( wait ) {
                        tmpstr[0] = *msg;
                        tmpstr[1] = EOS;
                        readuntil(fd, tmpstr, buf, 5);
                }
		msg++;
	}
}

/*
 *==========================================
 * setreg - preload a register through ODT
 *==========================================
 */
setreg(fd, reg,	value)
int fd,	reg, value;
{
	char buf[32];
	if (A.verbose)
		message("Setting register %d to 0%o",reg,value&0xffff);
	if ( reg >= 0 )
		sprintf(buf,"R%d/",reg);
	else
		sprintf(buf,"$S/");
	sendodt(fd,buf,TRUE);
	readuntil(fd," ",buf,5);
	sprintf(buf,"%o\r",value&0xffff);
	sendodt(fd,buf,TRUE);
	readuntil(fd,"@",buf,5);
}
/*
 *=================================================
 * message - conditionally display status message
 *=================================================
 */
message(str,arg,arg2)
char *str;
{
	if ( !A.silent ) {
		printf(str,arg,arg2);
		printf("\n");
	}
}
/*
 *=======================================
 * linemode - set up linemode and speed
 *=======================================
 */
linemode(fd,speed,mode)
int fd,	speed, mode;
{
	struct sgttyb tcb;
	if ( gtty(fd,&tcb) < 0 ) {
		perror(Openfiles[fd]);
		callexit(1);
	}
	if (speed != NOBAUD)
		tcb.sg_ispeed =	tcb.sg_ospeed =	speed;
	tcb.sg_flags = mode;
	stty(fd,&tcb);
}

/*
 *================================
 * gethdr - read an a.out header
 *================================
 */
gethdr(fd,hdr)
int fd;
struct exec *hdr;
{
	if ( read(fd, hdr, sizeof *hdr)	!= sizeof *hdr)	{
		fprintf(stderr,"%s: EOF	reading	a.out header\n",Openfiles[fd]);
		callexit(1);
	}
	if ( hdr->a_magic != A_MAGIC1 )	{
		fprintf(stderr,"%s: wrong magic	number\n",Openfiles[fd]);
		callexit(1);
	}
}

/*
 *============================================
 * displayval -	display	a number on one	line
 *============================================
 */
displayval(val)
int val;
{
	char buf[32];
        sprintf(buf,"%o ",val);
	printf("\r%s",buf);
	fflush(stdout);
}


/*
 *==================================================
 * fastread - receive packets from high	speed line
 *==================================================
 */
fastread(fd,ptr)
int fd;
lsiword	*ptr;
{
	struct packet packet;
	FILE *in;
	lsiword	sum;
	int i;
	in = fdopen(fd,"r");
	message("Reading LSI 11 memory...",0,0);
	do {
		do {
			getpack(in,&packet);
			if ( packet.len	<= MAXPACKSIZE ) {
				sum = packet.len + packet.base;
				for ( i=0 ; i<packet.len ; i++ )
					sum += packet.buf[i];
				if ( A.verbose )
					message("exp./actual sum=0%o/0%o",
						sum, packet.sum);
				if ( sum == packet.sum )
					break;
			}
			if ( !A.silent )
				fprintf(stdout,"NAK\n");
			write(fd,"n",1);
		} while	( TRUE ) ;
		write(fd,"y",1);
		for ( i=0 ; i<packet.len ; i++ )
			Memory[i+(packet.base/sizeof(lsiword))]	= packet.buf[i];
		if ( !A.silent )
			displayval((int)(packet.base&0xffff));
	} while	( packet.len !=	0 );
	if ( !A.silent ) {
		displayval(0);
		printf("\r");
	}
}
/*
 *=======================================
 * getpack - read next packet from line
 *=======================================
 */
getpack(fd,pptr)
FILE *fd;
struct packet *pptr;
{
	char c;
	Bool success;
	int i;
	/*
	 * wait	for start of heading character
	 */
	success	= FALSE;
	do {
		while (	(c=getc(fd)) !=	SOH ) ;	/* nil */
		if (A.verbose)
			fprintf(stdout,"SOH\n");
		pptr->len = getword(fd);
		pptr->base = getword(fd);
		if ( A.verbose )
			fprintf(stdout,"len, base = %o %o\n",pptr->len,
			pptr->base);
		if ( pptr->len > MAXPACKSIZE ) {
			return;
		}
		for ( i=0 ; i<pptr->len	; i++ )	{
			pptr->buf[i] = getword(fd);
		}
		pptr->sum = getword(fd);
		success	= TRUE;
	} while	( !success );
}
getword(fd)
FILE *fd;
{
	lsiword	word;
	word = getbyte(fd);
	word = word | (getbyte(fd)<<8);
	return(word);
}
getbyte(fd)
FILE *fd;
{
	char c;
	c = getc(fd);
if (A.verbose) fprintf(stdout,"%03o ",c&0xff);
	if ( c==ESC ) {
		c = getc(fd);
if (A.verbose) fprintf(stdout,"(%03o) ",c&0xff);
	}
	return(c&0xff);
}
