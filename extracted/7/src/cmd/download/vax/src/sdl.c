/*----------------------------------------------------------------------*/
/*									*/
/*	sdl	--  Download to VAX over Serial Line			*/
/*									*/
/* This program takes VAX a.out format files and downloads them		*/
/* into a VAX.								*/
/*									*/
/* Usage:								*/
/*  (This program is exec'd by download; the following arguments are	*/
/*   accepted by download:)						*/
/*  download [file] [-cCLASS] [-lLSI#] [-a[delay]]			*/
/*	     [-Bbaudrate] [-a[# of Secs]] [-h] [-v] [-s]		*/
/*									*/
/* Author:								*/
/*	Tom Stonecypher							*/
/*		based on the PDP-11 downloader, written by:		*/
/*		R. Brown, Doug Comer, D. Hachigian			*/
/*		Purdue University CS Deptartment			*/
/*									*/
/* (c) Copyright, 1986, All rights reserved.				*/
/*									*/
/*----------------------------------------------------------------------*/

/*    Include files - Unix domain                           		*/
#include "/usr/include/errno.h"
#include "/usr/include/setjmp.h"
#include "/usr/include/signal.h"
#include "/usr/include/sgtty.h"
#include "/usr/include/stdio.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/types.h"
#include "/usr/include/ctype.h"
#include "/usr/include/a.out.h"

/*    Include files - XINU domain                           		*/
#include <getdev.h>
#include <download.h>
#include <vaxdl.h>
#include <ethdown.h>

struct	arglist A;
jmp_buf	env;			/* used to return from read time-outs	*/
static	char *alarm_msg;	/* message printed when alarm signalled	*/
char	*Openfiles[MAXFILES];	/* open file names for error reporting	*/
char	*locknm;		/* name of lock file			*/
extern	int errno;

/*
 * MAIN -- download user's program from host into VAX's memory
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	struct	exec aouthdr;

	locknm = argv[--argc];
	machine = argv[--argc];
					/* cputype = get_cputype(machine); */
	procargs(argc, argv);		/* process arguments into struct A */
	initsigs();

	Openfiles[DEVFD] = get_name(machine);
	Openfiles[AOUTFD] = A.filename;

	gethdr(AOUTFD, &aouthdr, TRUE);

	serial_load(AOUTFD, LOADSTART, &aouthdr, A.startdelay);
	touch(locknm);
	message("done         ");
	printstart(&aouthdr);
	callexit(0);
}

/*
 *====================================================================
 * procargs - parse and check command line, setup global A structure
 *====================================================================
 */
procargs(argc, argv)
int	argc;
char	*argv[];
{
	int arg, i;

	A.filename  = DEFFILE;
	A.baudrate = B9600;
	A.class = NULL;
	A.machnum = GD_ANYDEV;
	A.devflags = FALSE;
	A.reloadether = FALSE;
	A.verbose = FALSE;
	A.silent = FALSE;
	A.autostart = FALSE;
	A.startdelay = NOAUTOSTART;

	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			switch ( argv[arg][1] ) {
			case 'l':
				A.machnum=atoi(argv[arg]+2);
				break;
			case 'h':
				A.devflags |= GD_HLDDEVS;
				break;
			case 'e':
				A.reloadether = TRUE;
				break;
			case 'v':
				A.verbose = TRUE;
				break;
			case 's':
				A.silent = TRUE;
				break;
			case 'a':
				A.autostart = TRUE;
				A.startdelay = atoi(&argv[arg][2]);
				break;
			default:
				break; /* ignore other args */
			}
		} else
			A.filename = argv[arg];
	}
}

/*
 *=================================================================
 * initsigs - prepare to reset on an interrupt & setup for alarms
 *=================================================================
 */
initsigs()
{
	int alarmhandler(), inthandler();

	if ( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal(SIGINT,inthandler);
	if ( signal(SIGTERM,SIG_IGN) != SIG_IGN )
		signal(SIGTERM,inthandler);
	if ( signal(SIGQUIT,SIG_IGN) != SIG_IGN )
		signal(SIGQUIT,inthandler);
	signal(SIGALRM, alarmhandler);
}

/*=======================================================================
 * serial_load - download an a.out image over serial line
 *=======================================================================
 * input  state: <undefined>
 * output state: a.out in memory, executing autostart else cpu halted
 */
serial_load(aout_fd, load_addr, aout_hdr, delay)
int	aout_fd;
char	*load_addr;
struct	exec	*aout_hdr;
int	delay;
{
	int	ldrfd;
	struct	exec ldr_hdr;

	ldrfd = openfile(VAXLOADER, O_RDONLY);
	gethdr(ldrfd, &ldr_hdr, FALSE);
	initcpu();
	message("loading boot program...");
	bootload(aout_hdr, ldrfd, &ldr_hdr, delay);
	slowload(aout_fd, aout_hdr, load_addr);
	close(ldrfd);
}

/*
 *=============================================
 * bootload - load bootstrap loader via console
 *=============================================
 * input state: after console prompt, cpu initialized (interrupts disabled)
 * output state: same, bootstrap loader in memory and running
 */
bootload(aout_hdr, fdin, ldr_hdr, delay)
int	fdin, delay;
struct	exec  *aout_hdr, *ldr_hdr;
{
	FILE *infile;
	int length, stklen, i, ph0len;
	unsigned long val;
	char buf[32], *stack, ch, resp;

	infile = fdopen(dup(fdin),"r");
	fseek(infile, N_TXTOFF(*ldr_hdr),0);
	length = ldr_hdr->a_text + ldr_hdr->a_data;
	/*
	 * load phase1 of the boot loader at location P0BSTRT.
	 */
	fread (&val, sizeof val, 1, infile);
	sprintf(buf, "D/L/P %x %x\r",P0BSTRT, val); /* initialize console */
	sendcons(buf, TRUE);
	getprompt(6);
	fread ( &val, sizeof val, 1, infile);
	ph0len = sizeof (unsigned long) * 2;
	while ( val != (unsigned)0xffffffff ) {
		sprintf(buf, "D + %x\r", val);
		sendcons(buf, TRUE);
		getprompt(6);
		if ( !A.silent && ! A.verbose )
			displayval(ph0len);
		fread ( &val, sizeof val, 1, infile);
		ph0len += sizeof (unsigned long);
	}
	if ( !A.silent && !A.verbose )
                fprintf(stderr,"\r");
	sendcons("\r",TRUE);
	getprompt(5);
	/*
	 * send the boot loader in reverse order, remove trailing zeros
	 */
	stack = (char *)malloc(length);
	for (stklen = 0; ph0len+stklen < length; stklen++)
		stack[stklen] = getc(infile);
	setreg(0, stklen);
	setreg(1, HIADDR);
	setreg(2, delay);
	setreg(3, aout_hdr->a_entry);
	sprintf(buf, "S %x\r", P0BSTRT);
        sendcons(buf, TRUE);
	usleep(1000000);
	buf[0] = ESC;
	/* escape bytes for big VAXen */
	for ( i=stklen-1 ; i>=0 ; i-- ) {
		if( (ch = stack[i])==CNSINTRC || ch==CNSINTRC2 || ch==ESC ) {
			buf[1] = ch|STUFFBIT;
			write(DEVFD, buf, 2);
		} else
			write(DEVFD, &ch, 1);
		if ( A.verbose )
			fprintf(stderr,"%03x ",stack[i]&0xff);
	}
        if ( A.verbose )
                fprintf(stderr,"\n");
}

/*
 *============================================
 * getserack - get an ack from destination VAX
 *============================================
 */
getserack()
{
	char ch;

	settimer(6, "getack read timed out");
	if (setjmp(env)	== EINTR)
		callexit(1);
	else if (read(DEVFD, &ch, 1) < 0) {
		perror(Openfiles[DEVFD]);
		message("getserack: no ack");
		ch = EOS;
	} else
		canceltimer();
	if (A.verbose)
		fprintf(stderr,"IN(getack): %s\n", unctrl(ch));
	return(ch);
}

FILE *Slowline;
char Slowbuf[BUFSIZ];

/*============================================================
 * slowload - load user program via packets over serial line
 *============================================================
 * input  state: serial downloader running, ready to receive packets
 * output state: a.out file loaded into memory; a.out running if auto-
 *		 start, before console prompt if no autostart
 */
slowload(aoutfd, ahdr, load_addr)
int	aoutfd;
struct	exec *ahdr;
int	load_addr;
{
        int words, addr, length;
	FILE *infile;
	unsigned short buf[PACKSIZE];
	char resp;

	words = (ahdr->a_text+ahdr->a_data) / sizeof (unsigned short);
	Slowline = fdopen(DEVFD,"w");
	setbuf(Slowline,Slowbuf);
	infile = fdopen(aoutfd,"r");
	fseek(infile, N_TXTOFF(*ahdr), 0);
	addr = load_addr;
	while ( words > 0 ) {
		length = (words>PACKSIZE ? PACKSIZE : words);
		fread(buf,sizeof(unsigned short),length,infile);
		do {
			sendserpack(addr,length,buf);
			settimer(6, "slowload read timed out");
			if (setjmp(env)	== EINTR)
				callexit(1);
			if ( read ( DEVFD, &resp, 1) != 1 ) {
				perror(Openfiles[DEVFD]);
				exit(1);
			}
			canceltimer();
			if ( A.verbose )
			     fprintf(stderr,"response is %s\n",unctrl(resp));
			else if (resp == NAK )
				fprintf(stderr,"NAK\n");
		} while ( resp != ACK );

		addr += length * sizeof (unsigned short);
		words -= length;
                if ( !A.silent && !A.verbose )
                        displayval(words);
	}
        if ( !A.silent && !A.verbose ) {
                displayval(0);
                fprintf(stderr,"\r");
        }
	do {
	       sendserpack ( addr, 0, buf );
	} while (getserack() != ACK);
}

/*
 *====================================
 * sendserpack - send a packet to the VAX
 *====================================
 */
#define putwsum(w) putw(w), sum += w
#define rawputc(c) putc(c, Slowline)

sendserpack(addr, length, buf)
unsigned int addr, length;
unsigned short *buf;
{
short	sum;
int	i;

	if ( A.verbose ) {
		fprintf(stderr,"Sending packet addr=%8x len=%d\n",addr,length);
		for ( i=0 ; i<length ; i++ )
			fprintf(stderr,"%02x  ",buf[i]&0xff);
		fprintf(stderr,"\n");
        }
	rawputc(SOH);
	sum = 0;
	putwsum(addr & LOW16);		/* put low word of address	*/
	putwsum(addr>>16);		/* put high word of address	*/
	putwsum(length);
	for ( i=0 ; i<length ; i++ )
		putwsum(buf[i]);
        if ( A.verbose )
		fprintf(stderr, "sum is %d\n", -sum);
        putwsum(-sum);
	fflush(Slowline);
}

/*
 *==================================================
 * putw - write a word to the slow-speed serial line
 *==================================================
 */
putw(word)
unsigned short word;
{
	putch((char)(word&LOWBYTE));
	putch((char)((word>>8)&LOWBYTE));
}

/*
 *==============================================================
 * putch - put a character on the high speed line, with escaping
 *==============================================================
 */
putch(ch)
char ch;
{
	if ( ch == ESC || ch == SOH || ch == CNSINTRC || ch == CNSINTRC2 ) {
		rawputc(ESC);
		rawputc(ch|STUFFBIT);
	} else
		rawputc(ch);
}

/*
 *===================================================
 * printstart - print start location for serial loads
 *===================================================
 */
printstart(hdr)
struct exec *hdr;
{
	if ( !A.autostart && !A.silent )
		fprintf(stderr,"\rHalt the cpu, then start at 0x%x\n",
			  hdr->a_entry+sizeof(short));
}

/*
 *================================
 * gethdr - read an a.out header
 *================================
 */
gethdr(fd, hdr, cfile)
int	fd;
struct	exec *hdr;
Bool	cfile;
{
	if ( read(fd, hdr, sizeof *hdr) != sizeof *hdr) {
		fprintf(stderr,"%s: EOF reading a.out header\n",Openfiles[fd]);
		callexit(1);
	}
	if (cfile && hdr->a_magic != NMAGIC && hdr->a_magic != ZMAGIC ) {
		fprintf(stderr,"%s: wrong magic number\n",Openfiles[fd]);
		callexit(1);
	}
}

/*
 *==========================================================
 * openfile - try to open file, exit with message if error
 *==========================================================
 */
openfile(name,mode)
char	*name;
int	mode;
{
	int fd;
	settimer(10, "open timed out");
	if (setjmp(env) == EINTR)
		callexit(1);
	if ((fd=open(name,mode)) < 0 ) {
		perror(name);
		callexit(1);
	}
	canceltimer();
	Openfiles[fd] = name;
	return(fd);
}

/*
 *==================================================
 * readuntil - read from line until some character
 *==================================================
 */
readuntil ( ch, time )
char	ch;
int	time;
{
	char	input;

	settimer(time, "\rread timed out");
	if (setjmp(env)	== EINTR) {
		fprintf(stderr, "readuntil did not find %s\n", unctrl(ch));
		callexit(1);
	}
	if ( A.verbose ) {
		fprintf(stderr,"IN: "); fflush(stderr);
	}
	do {
		if ( read(DEVFD, &input, 1) != 1 ) {
			perror(Openfiles[DEVFD]);
			callexit(1);
		}
		if (A.verbose ) {
			fprintf(stderr,"%s",unctrl(input)); fflush(stderr);
		}
	} while ( input == EOS || input != ch);
	canceltimer();
	if ( A.verbose )
		fprintf(stderr,"\n");
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
	touch(locknm);
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
	exit(ret);
}

/*
 *==================================================
 * sendcons - send a message to console half duplex
 *==================================================
 */
sendcons(msg, wait)
char *msg;
Bool wait;
{
	char	*ptr;

	ptr = msg;
	while (*msg!=EOS) {
		if (islower(*msg))
			*msg = *msg - 32;
		write(DEVFD,msg,1);
                if ( wait )
                        readuntil(*msg, 6);
		msg++;
	}
	if ( A.verbose ) {
		fprintf(stderr,"OUT: ");
		while (*ptr!=EOS) {
			fprintf(stderr,"%s",unctrl(*ptr));
			ptr++;
		}
		fprintf(stderr,"\n");
	}
}

/*
 *================================================
 * setreg - preload a register through VAX console
 *================================================
 */
setreg(reg, value)
int reg, value;
{
	char buf[32];

	/* there's no machine-independent way to load PSL, so let	*/
	/* console Initialize command take care of that			*/
	if (reg == KSPREG)
		sprintf (buf, "D/I 0 %x\r", value);
        else if ( reg >= 0 ) /* general register */
                sprintf(buf,"D/G %x %x\r", reg, value);
	else /* internal privileged register -reg */
		sprintf(buf, "D/I %x %x\r", -reg, value);
	sendcons(buf, TRUE);
        if ( !A.silent && !A.verbose )
                displayreg(reg);
	getprompt (6);
}

/*
 *===================================
 * getprompt - get VAX console prompt 
 *===================================
*/
getprompt (time)
int time;
{
	int i;

	for (i=0; i<3; i++)
		readuntil ('>', time);
}
	
/*
 *======================================================
 * initcpu - get VAX console's attention, initialize cpu
 *======================================================
 * Input state - undefined
 * Output state - immediately after VAX console prompt
 */
initcpu()
{
	message("getting console");

	/* get console, if it's a big VAX */
	write(DEVFD, CONSKILL, 1);	/* ^U erases current cmd. line	*/
	write(DEVFD, CONSABRT, 1);	/* ^C enables console input	*/
	write(DEVFD, CONSINTR, 1);	/* ^P breaks to console mode	*/
	usleep(100000);

	/* get console, if it's a UVAX */
	ioctl(DEVFD, TIOCSBRK, NULL);
	usleep(300000);
	ioctl(DEVFD, TIOCCBRK, NULL);
	/* now have console, initialize cpu */
	getprompt(15);
	sendcons(CPUHALT, TRUE);	/* halt the processor		*/
	sendcons("\r", TRUE);
	getprompt(6);
	sendcons(CPUNJAM, TRUE);	/* unjam bus			*/
	getprompt(15);			/* high waits for big VAXen	*/
	sendcons(CPUINIT, TRUE);	/* init: interrupts disab, etc.	*/
	getprompt(15);			/* takes a while on big VAXen	*/
}

/*
 *============================================================
 * setmem - load a 32-bit memory location through VAX console
 *============================================================
 */
setmem(loc, value)
int loc, value;
{
	char buf[128];

        sprintf(buf,"D/L/P %x %x\r",loc,value);
	sendcons(buf, TRUE);
	getprompt(6);
}

/*
 *=================================================
 * message - conditionally display status message
 *=================================================
 */
message(str)
char *str;
{
	if ( !A.silent ) {
		fprintf(stderr,"\r%s\n",str);
	}
}


/*
 *============================================
 * displayval - display a number on one line
 *============================================
 */
displayval(val)
int val;
{
	fprintf(stderr,"\r%d     ",val);
	fflush(stderr);
}

/*
 *==============================================
 * displayreg - display a register on one line
 *==============================================
 */
displayreg(val)
int val;
{
        if ( val < 0 )
		fprintf(stderr,"\rPS           ");
        else
		fprintf(stderr,"\rR%d          ",val);
	fflush(stderr);
}
