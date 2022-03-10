/*----------------------------------------------------------------------*/
/*									*/
/*	sul	--  Upload from PDP-11 to UNIX machine using		*/
/*		    a serial line					*/
/* This program uploads a memory image from a PDP-11 into a core11 file	*/
/* on a UNIX machine.							*/
/*									*/
/* Configuration -							*/
/*	VAX tty line connected to LSI 11 ODT (console) port		*/
/*									*/
/* Usage:								*/
/*	upload [-lx] [-o filename] [ [minaddr] max addr ]		*/
/*									*/
/* Authors:								*/
/*	Version 1 : Serial Uploader					*/
/*		R. Brown & C. Kent					*/
/*		Purdue University/CS Department				*/
/*		September, 1981						*/
/*		mods by Comer, 11/81-11/83				*/
/*	Version 2 : modularized to be exec'd by generic uploader	*/
/*		Tom Stonecypher						*/
/*		Purdue University CS Department				*/
/*		November, 1986						*/
/*									*/
/*----------------------------------------------------------------------*/

/* UNIX include files */
#include "/usr/include/stdio.h"
#include "/usr/include/sgtty.h"
#include "/usr/include/signal.h"
#include "/usr/include/errno.h"
#include "/usr/include/setjmp.h"
#include "/usr/include/errno.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/ctype.h"

/* Xinu domain include files */
#include <core11.h>
#include <a.out.h>
#include <baud.h>
#include <getdev.h>
#include <ethdown.h>
#include <lsiul.h>
#include <dlpack.h>
#include <upload.h>

char 	*msgval;
char	*Openfiles[20];
struct	core11	Corehead;
jmp_buf	env;			/* environment to return to on timeout	*/
static	char *alarm_msg;	/* message printed when alarm signalled	*/
struct	arglist	A;		/* parsed argument values		*/
char	*lockfile;		/* path name of lockfile		*/
int	Haltedat;
struct	sgttyb	VAXttyb;
char	*Memory;		/* the LSI-11 memory image uploaded	*/
char	*index();
int	touch();
extern	int	errno;

/*
 * MAIN -- upload memory image from PDP-11 to UNIX machine
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	u_long	lsi_ip;
	int	i, core_fd;
	struct	exec ehdr, aouthdr;

	lockfile = argv[--argc];
	machine = argv[--argc];
					/* cputype = get_cputype(machine); */
	procargs(argc, argv);		/* process arguments into struct A */
	initsigs();
	if ((core_fd = creat(A.corefile, 0666)) < 0) {
		perror(A.corefile);
		exit(1);
	}
	dup2(core_fd, COREFD);	/* write to the core file using COREFD	*/
	Openfiles[DEVFD] = get_name(machine);
	Openfiles[COREFD] = A.corefile;

	/*
	 * allocate memory array - this	written	to the core file when the
	 * program terminates
	 */
	if (A.verbose)
		fprintf(stderr,"Allocating %d (0%o) bytes for image\n",
			MAXADDR+1, MAXADDR+1);
	Memory = (char *)malloc(MAXADDR+1);
	for ( i=0 ; i <= MAXADDR ; i++ )
		Memory[i] = (char)0;
	Corehead.c_zero1 = Corehead.c_zero2 = 0;
	Corehead.c_zero3 = Corehead.c_zero4 = Corehead.c_zero5 = 0;
	Corehead.c_magic = COREMAGIC;

	seruload();
	write(COREFD, &Corehead, sizeof Corehead);
	write(COREFD, Memory, A.highaddr+1);
	touch(lockfile);
	message("\rdone     \n",0,0);
	callexit(0);
}

/*----------------------------------------------------------------------
 * initsigs -- trap software signals so that lines can be restored to
 * their original state and the core file can be written
 *----------------------------------------------------------------------
*/
initsigs()
{
	int	alarmhandler(), inthandler();

	if ( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal(SIGINT,inthandler);
	if ( signal(SIGTERM,SIG_IGN) !=	SIG_IGN	)
		signal(SIGTERM,inthandler);
	if ( signal(SIGQUIT,SIG_IGN) !=	SIG_IGN	)
		signal(SIGQUIT,inthandler);
	signal(SIGALRM,	alarmhandler);
}

/*----------------------------------------------------------------------
 *  seruload -- upload an LSI 11 memory image serially
 *----------------------------------------------------------------------
 */
seruload()
{
	int ldrfd, bytes;
	int base,length,i;
	struct exec bhdr;
	short entry;

        message("Getting ODT...\n",0,0);
	getodt();
	/*
	 * Get the contents of the registers
	 */
	message("Getting registers...\n",0,0);
	getregs(DEVFD,Corehead.c_regs);
	getpsw(DEVFD,&Corehead.c_psw);
	if (A.verbose)
		message("Getting a.out header\n",0,0);
	ldrfd = openfile(A.bootloader, O_RDONLY);
	gethdr(ldrfd,&bhdr);
	bytes =	bhdr.a_text+bhdr.a_data+STACKLEN;
	if (A.verbose)
		message("Number of bytes = %d\n",bytes,0);
	if ( !A.noload ) {
		message("Deposit boot loader...\n",0,0);
		entry =	odtload(ldrfd,&bhdr,0,Memory);
	} else {
		message("Not loading boot loader...\n",0,0);
		read ( ldrfd, Memory, bytes*sizeof(lsiword) );
	}
	/*
	 * Set up arguments to upl and start it	running...
	 */
	if (A.verbose)
		message("Getting ODT again...\n",0,0);
	getodt();
	message("Starting load of 0%o-0%o...\n",A.lowaddr,A.highaddr);
	base = (A.lowaddr < bytes ? bytes :	A.lowaddr);
	length = (A.highaddr-base + 1)/2;
	if (A.verbose)
		message("Setting argument registers\n",0,0);
	setreg(0,base);
	setreg(1,length);
	setreg(4,CSRBASE);
	setreg(6,base);
	if (A.verbose)
		message("Starting LSI 11 execution...\n",0,0);
	sendodt("P",TRUE);
	slowread(Memory);
}

/*
 *======================================================================
 * procargs - parse and check arguments, modifying global A structure
 *======================================================================
 */
procargs(argc, argv)
int argc;
char *argv[];
{
	int	arg, i, unswitched, afd;
	struct	exec	ahdr;
	/*
	 * assign default values
	 */
	A.uploader = NULL;
	A.lowaddr = 0;
	A.highaddr = MAXADDR;
	A.corefile = DEFFILE;
	A.baudrate = B9600;
	A.class = NULL;
	A.machnum = GD_ANYDEV;
	A.devflags = GD_MYDEV;
	A.bootloader = LOADER;
	A.verbose = FALSE;
	A.silent = FALSE;
	A.noload = FALSE;
	unswitched = 0;
	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' )
			switch ( argv[arg][1] ) {
			case 'v':
				A.verbose = TRUE;
				break;
			case 's':
				A.silent = TRUE;
				break;
			case 'n':
				A.noload = TRUE;
				break;
			case 'o':
				A.corefile = argv[arg++];
				break;
			case 'a':
					afd = openfile(argv[arg++],0);
					gethdr(afd,&ahdr);
					A.lowaddr = ahdr.a_text;
					A.highaddr = ahdr.a_text+ahdr.a_data+
							ahdr.a_bss;
					break;
			default:	/* generic uploader already parsed: */
				break;	/* don't complain about arguments */
			}
	else /* there's no dash in front */
		switch ( unswitched++ )	{
		case 0:
			A.highaddr = atoi(argv[arg]);
			message("highaddr=%o\n",A.highaddr,0);
			break;
		case 1:
			A.lowaddr = A.highaddr;
			A.highaddr = atoi(argv[arg]);
			message("lowaddr=%o\n",A.lowaddr,0);
			message("highaddr=%o\n",A.highaddr,0);
			break;
		default:
			break;		/* don't complain -- already parsed */
		}
	}
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
	if (setjmp(env) == EINTR)
		callexit(1);
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
 * alarmhandler - return from alarm calls
 *=========================================
 */
alarmhandler()
{
	signal(SIGALRM, alarmhandler);
	if (alarm_msg != NULL)
		fprintf(stderr, "%s\n", alarm_msg);
	longjmp(env, EINTR);
	/* not reached */
}

/*
 *=====================================
 * inthandler -- interrupt processing
 *=====================================
 */
inthandler()
{
	write(COREFD,&Corehead,sizeof Corehead);
	write(COREFD,Memory,A.highaddr+1);
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
	/* stty(DEVFD,&VAXttyb); */
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
	setbuf(stderr,NULL);
	setbuf(infile,filebuf);
	length = hdr->a_text+hdr->a_data+STACKLEN;
	addr = LOADSTART;
	do {
		fread(&word, sizeof word, 1, infile);
		if ( addr == LOADSTART ) {
			sprintf(buf,"%o/",addr+loc);
			sendodt(buf,TRUE);
			readuntil(" ?", buf, 7, 32 );
			val = buf;
		} else {
			sendodt("\n",TRUE);
			readuntil(" ?", buf, 7, 32);
			val = index(buf,'/')+1;
		}
		if ( outaddr !=	NULL ) {
			sscanf(val,"%o ",&lsival);
			outaddr[(addr+loc)/2] =	lsival & 0xffff;
		}
		sprintf(buf, "%o", word	& 0xffff);
		sendodt(buf,TRUE);
		if ( !A.silent && !A.verbose )
			displayval(addr);
		addr +=	sizeof (lsiword);
	} while	( word != 0177777 );
	if ( !A.silent && !A.verbose )
		fprintf(stderr,"\r");
	sendodt("\r",TRUE);
	readuntil("@", buf, 5, 32);
	/*
	 * now,	run this half to load the other half faster
	 */
	fseek(infile,sizeof(struct exec),0);
	setreg(1,length);
	setreg(-1,0340);
	setreg(7,LOADSTART);
	sendodt("P",FALSE);
        sleep(1);
	for ( i=0 ; i<length ; i++ ){
		c = getc(infile);
		if (A.verbose){
			fprintf(stderr,"AT: %06o ", i&0xffff);
			fprintf(stderr,"OUT: %03o ",c&0xff);
		}
		write(DEVFD,&c,1);
		read(DEVFD,&c,1);
		if (A.verbose) fprintf(stderr,"IN: %03o\n",c&0xff);
		if ( i>=addr && outaddr != NULL )
			*(((char *)outaddr)+i) = c;
	}
	readuntil("@", buf, 8, 32);
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

	sendodt("$S/",TRUE);
	readuntil(" ?", buf, 5, 60);
	reply = index(buf,'/');
	sscanf( ((0==(int)reply) ? buf : reply+1),"%o ",psw);
	sendodt("\r",TRUE);
	readuntil("@", buf, 5, 60);
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
			sendodt(buf,TRUE);
			readuntil(" ?", buf, 7, 32);
			val = buf;
		} else {
			sendodt( "\n",TRUE);
			readuntil(" ?", buf, 7, 32);
			val = index(buf,'/')+1;
		}
		sscanf(val,"%o ",&lsival);
		regs[r]	= lsival & 0xffff;
		if ( !A.silent && !A.verbose )
			displayval(r);
	}
	if ( !A.silent && !A.verbose )
		fprintf(stderr,"\r");
	sendodt("\r",TRUE);
	readuntil("@", buf, 5, 32);
}

/*
 *===============================
 * getodt - get ODT's attention
 *===============================
 * Input state - undefined -- nothing valid in memory
 * Output state - immediately after ODT prompt
 */
getodt()
{
	char buf[32];

	ioctl(DEVFD, TIOCSBRK, NULL);
	usleep(300000);
	ioctl(DEVFD, TIOCCBRK, NULL);
	readuntil("@",buf,10,32);
}

/*
 *==================================================
 * readuntil - read from line until some character
 *==================================================
 */
readuntil ( ch, buf, time, nchars )
int time, nchars;
char *ch, *buf;
{
	settimer(time, "read timed out");
	if (setjmp(env) == EINTR) {
		fprintf(stderr, "readuntil did not find %s\n", unctrl(*ch));
		callexit(1);
	}
	if ( A.verbose ) {
		fprintf(stderr,"IN: "); fflush(stderr);
	}
	do {
		if ( read(DEVFD, buf, 1) != 1 ) {
			perror(Openfiles[DEVFD]);
			callexit(1);
		}
		if (A.verbose )
			fprintf(stderr,"%s",unctrl(*buf));
	} while	( (*buf == '\0' || index(ch,*buf++) == NULL) &&
		  --nchars > 0 );
	*buf = EOS;
	canceltimer();
	if ( A.verbose )
		fprintf(stderr,"\n");
}

/*
 *==============================================
 * sendodt - send a message to ODT half duplex
 *==============================================
 */
sendodt(msg, wait)
char *msg;
Bool wait;
{

	char buf[32], *ptr, tmpstr[2];

	if ( A.verbose ) {
		fprintf(stderr,"OUT: ");
		for ( ptr=msg ; *ptr!=EOS ; ptr++ )
			fprintf(stderr,"%s",unctrl(*ptr));
		fprintf(stderr,"\n");
	}
	while (*msg!=EOS) {
		write(DEVFD,msg,1);
                if ( wait ) {
			tmpstr[0] = *msg;
			tmpstr[1] = EOS;
                        readuntil(tmpstr, buf, 5, 32);
		}
		msg++;
	}
}

/*
 *==========================================
 * setreg - preload a register through ODT
 *==========================================
 */
setreg(reg, value)
int reg, value;
{
	char buf[32];

	if (A.verbose)
		message("Setting register %d to 0%o\n",reg,value&0xffff);
	if ( reg >= 0 )
		sprintf(buf,"R%d/",reg);
	else
		sprintf(buf,"$S/");
	sendodt(buf,TRUE);
	readuntil(" ?", buf, 5, 32);
	sprintf(buf,"%o\r",value&0xffff);
	sendodt(buf,TRUE);
	readuntil("@", buf, 5, 32);
}

/*
 *=================================================
 * message - conditionally display status message
 *=================================================
 */
message(str,arg,arg2)
int	arg, arg2;
char	*str;
{
	if ( !A.silent && !A.verbose)
		fprintf(stderr,str,arg,arg2);
	fflush(stderr);
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
	fprintf(stderr,"\r%s",buf);
	fflush(stderr);
}


/*
 *======================================================
 * slowread - receive packets from low speed serial line
 *======================================================
 */
slowread(mem)
lsiword	mem[];				/* treat memory as array of	*/
{					/* shorts (historical)		*/
	struct spkt packet;
	FILE *in;
	lsiword	sum;
	int i;
	in = fdopen(DEVFD,"r");
	message("Reading LSI 11 memory...\n",0,0);
	do {
		do {
			getpack(in,&packet);
			if ( packet.len	<= MAXSERPKT ) {
				sum = packet.len + packet.base;
				for ( i=0 ; i<packet.len ; i++ )
					sum += packet.buf[i];
				if ( A.verbose )
					message("exp./actual sum=0%o/0%o\n",
						sum, packet.sum);
				if ( sum == packet.sum )
					break;
			}
			if ( !A.silent )
				fprintf(stderr,"NAK\n");
			write(DEVFD,"n",1);
		} while	( TRUE ) ;
		write(DEVFD,"y",1);
		for ( i=0 ; i<packet.len ; i++ )
			mem[i+(packet.base/sizeof(lsiword))] = packet.buf[i];
		if ( !A.silent )
			displayval((int)(packet.base&0xffff));
	} while	( packet.len !=	0 );
	if ( !A.silent ) {
		displayval(0);
		fprintf(stderr,"\r");
	}
}

/*
 *=======================================
 * getpack - read next packet from line
 *=======================================
 */
getpack(fd,pptr)
FILE *fd;
struct spkt *pptr;
{
	char c;
	Bool success;
	int i;
	/*
	 * wait	for start of heading character
	 */
	success	= FALSE;
	do {
		while (	(c=getc(fd)) !=	SOH )
			;	/* NULL */
		if (A.verbose)
			fprintf(stderr,"SOH\n");
		pptr->len = getword(fd);
		pptr->base = getword(fd);
		if ( A.verbose )
			fprintf(stderr,"len, base = %o %o\n",pptr->len,
			pptr->base);
		if ( pptr->len > MAXSERPKT )
			return;
		for ( i=0 ; i<pptr->len	; i++ )
			pptr->buf[i] = getword(fd);
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
	if (A.verbose)
		fprintf(stderr,"%03o ",c&0xff);
	if ( c==ESC ) {
		c = getc(fd);
		if (A.verbose)
			fprintf(stderr,"(%03o) ",c&0xff);
	}
	return(c&0xff);
}
