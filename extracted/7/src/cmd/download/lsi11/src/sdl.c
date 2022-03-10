/*----------------------------------------------------------------------*/
/*									*/
/*	sdl	-- Serial Downloader for LSI 11				*/
/*									*/
/* This program takes PDP-11 a.out format files and downloads them	*/
/* into a PDP-11.							*/
/*									*/
/* Usage:								*/
/*  (This program is exec'd by download; the following arguments are	*/
/*   accepted by download:)						*/
/*  download [file] [-cCLASS] [-lLSI#] [-a[delay]]			*/
/*	     [-Bbaudrate] [-a[# of Secs]] [-h] [-v] [-s]		*/
/*									*/
/* Authors:								*/
/*	Version One: R. Brown, Doug Comer				*/
/*	Purdue University/CS Deptartment				*/
/*	September, 1981 - Nov. 1983					*/
/*	Fixed several bugs (Tom Stonecypher, 8/86), made major mods:	*/
/*		longjmp botches fixed, output format fixed, power-up	*/
/*		initialization of memory-map unit fixed			*/
/*		modified to allow download to exec this program		*/
/*									*/
/* (c) Copyright, 1986, All rights reserved.				*/
/*									*/
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------*/
/*    Include files - Unix domain                           */
/*----------------------------------------------------------*/
#include "/usr/include/errno.h"
#include "/usr/include/setjmp.h"
#include "/usr/include/signal.h"
#include "/usr/include/sgtty.h"
#include "/usr/include/stdio.h"
#include "/usr/include/sys/file.h"
#include "/usr/include/sys/types.h"

/*----------------------------------------------------------*/
/*    Include files - XINU domain                           */
/*----------------------------------------------------------*/
#include <a.out.h>
#include <getdev.h>
#include <download.h>
#include <lsidl.h>

struct	arglist A;
jmp_buf	env;			/* used to return from read time-outs	*/
static	char *alarm_msg;	/* message printed when alarm singaled	*/
char	*Openfiles[MAXFILES];	/* open file names for error reporting	*/
char	*locknm;
extern	int errno;

/*
 * MAIN -- download user's program from host into LSI's memory
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	int	eloadfd;
	struct	exec ehdr, aouthdr;

	locknm = argv[--argc];
	machine = argv[--argc];
	/* cputype = get_cputype(machine); */
	procargs(argc, argv);		/* process arguments into struct A */
	initsigs();

	Openfiles[DEVFD] = get_name(machine);
	Openfiles[AOUTFD] = A.filename;

	gethdr(AOUTFD, &aouthdr);
	deposit_ldr();
	A.startdelay = A.autostart ? A.startdelay : NOAUTOSTART;
	start_ldr(&aouthdr, A.startdelay);
	fastload(&aouthdr);
	initstart(&aouthdr);

	touch(locknm);
	callexit(0);
}

/*
 *====================================================================
 * procargs - setup global A structure
 *====================================================================
 */
procargs(argc, argv)
int argc;
char *argv[];
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
	A.startdelay = 0;

	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			switch ( argv[arg][1] ) {
			case 'l':
				A.machnum=atoi(argv[arg]+2);
				break;
			case 'h':
				A.devflags |= GD_HLDDEVS;
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
				break; /* extra args ignored */
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
 * deposit_ldr - init so that bootload() can load the serial downloader
 *=======================================================================
 * input  state: <undefined>
 * output state: after odt prompt, serial packet downloader in memory
 */
deposit_ldr()
{
	int ldrfd;
	struct exec ldr_hdr;

	ldrfd = openfile(LSILOADER, O_RDONLY);
	gethdr(ldrfd,&ldr_hdr);
	getodt();
	message("\rloading boot program...");
	bootload(ldrfd, &ldr_hdr, 0);
	close(ldrfd);
}

/*
 *===========================================
 * bootload - load bootstrap loader via ODT
 *===========================================
 * input state: after ODT prompt
 * output state: same
 */
bootload(fdin, hdr, loc)
int fdin;
int loc;
struct exec *hdr;
{
	FILE *infile;
	int length, stklen, i, j, addr;
	lsiword word;
	char buf[32], *stack;

	infile = fdopen(dup(fdin),"r");
	fseek(infile,sizeof(struct exec),0);
	length = hdr->a_text + hdr->a_data;
	addr = LOADSTART;
	/*
	 * load first part of the boot loader at location LOADSTART
	 */
	do {
		fread ( &word, sizeof word, 1, infile);
		if ( addr == LOADSTART ) {
			sprintf(buf,"%o/",addr+loc);
			sendodt(buf,TRUE);
		} else
			sendodt("\n",TRUE);
		readuntil(' ', 7);
		sprintf(buf,"%o",word&0xffff);
		sendodt(buf,TRUE);
		if ( !A.silent && ! A.verbose )
			displayval(addr);
		addr += sizeof (lsiword);
	} while ( word != 0177777 );
	if ( !A.silent && ! A.verbose )
                fprintf(stderr,"\r");
	sendodt("\r",TRUE);
	readuntil('@',5);
	/*
	 * send the boot loader in reverse order
	 */
	stack = (char *)malloc(length);
	stklen = 0;
	while ( addr+stklen < length )
		stack[stklen++] = getc(infile);
	setreg(0,MAXADDR);
	setreg(1,stklen);
        setreg(7,LOADSTART);
        setreg(-1,INTDISABLE);
        sendodt("P", FALSE);
	sleep(1);
	for ( i=stklen-1 ; i>=0 ; i-- ) {
		write(DEVFD, &stack[i], 1);
		if ( A.verbose )
			fprintf(stderr,"%03o ",stack[i]&0xff);
	}
        if ( A.verbose )
                fprintf(stderr,"\r");
	readuntil('@',5);
}

/*==================================================================
 * start_ldr - load arguments to serial downloader and start it up
 *==================================================================
 * input  state: after odt prompt, serial loader in memory
 * output state: downloader running, ready to receive packets
 */
start_ldr(hdr, delay)
struct exec *hdr;
int delay;
{
	setreg(1, hdr->a_bss);
	setreg(2, delay);
	setreg(3, hdr->a_entry & (~1));
	setreg(4, LSIPORT);
        message("\rstarting boot loader...");
	sendodt("P", TRUE);
}

/*============================================================
 * fastload - load user program via packets over serial line
 *============================================================
 * input  state: serial downloader running, ready to receive packets
 * output state: a.out file loaded into memory; a.out running if auto-
 *		 start, before odt prompt if no autostart
 */

FILE *Fastline;
char Fastbuf[BUFSIZ];

fastload(ahdr)
struct exec *ahdr;
{
        int words, addr, length, total;
	FILE *infile;
	lsiword buf[PACKSIZE];
	char resp;

	words = (ahdr->a_text+ahdr->a_data) / sizeof (lsiword);
	Fastline = fdopen(DEVFD,"w");
	setbuf(Fastline,Fastbuf);
	infile = fdopen(AOUTFD,"r");
	fseek(infile, (long) sizeof (struct exec), 0);
	addr = LOADSTART;
	while ( words > 0 ) {
		length = (words>PACKSIZE ? PACKSIZE : words);
		fread(buf,sizeof(lsiword),length,infile);
		do {
			sendpack(addr,length,buf);
                        settimer(6, "fastload read timed out");
			if (setjmp(env) == EINTR)
				callexit(1);
                        if ( read ( DEVFD, &resp, 1) != 1 ) {
				perror(Openfiles[DEVFD]);
				exit(1);
                        }
                        canceltimer();
                        if ( A.verbose )
                                fprintf(stderr,"response is %s\n",unctrl(resp));
                        else if ( resp == NAK )
                                fprintf(stderr,"NAK\n");
		} while ( resp != ACK );

		addr += length * sizeof (lsiword);
		words -= length;
                if ( !A.silent && !A.verbose )
                        displayval(words);
	}
        if ( !A.silent && !A.verbose ) {
                displayval(0);
                fprintf(stderr,"\r");
        }

	do {
	        sendpack ( addr, 0, buf );
	} while (getack() != ACK);
}

/*
 *=================================================================
 * initstart - initialize startup register value for serial loads
 *=================================================================
 * input  state: before odt prompt
 * output state: after odt, a.out setup to run
 */
initstart(hdr)
struct exec *hdr;
{
	int i;

	if ( !A.autostart ) {
		readuntil('@', 2);
		setreg(7, (i=hdr->a_entry & (~1)) );
		setreg(6, hdr->a_text + hdr->a_data + hdr->a_bss + 6);
		setreg(-1, INTDISABLE);
		if ( !A.silent )
			fprintf(stderr,"\nStart at 0%o\n",i);
	}
}

/*
 *=================================
 * getack - get an ack from lsi
 *=================================
 */
getack()
{
	char ch;

	settimer(6, "getack read timed out");
	if (setjmp(env)	== EINTR)
		return(NULL);
	else if (read(DEVFD, &ch, 1) < 0) {
		perror(Openfiles[DEVFD]);
		message("getack: no ack");
		ch = '\0';
	} else
		canceltimer();
	if (A.verbose)
		fprintf(stderr,"IN(getack): %s\n", unctrl(ch));
	return(ch);
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
	if ( read(fd, hdr, sizeof *hdr) != sizeof *hdr) {
		fprintf(stderr,"%s: EOF reading a.out header\n",Openfiles[fd]);
		callexit(1);
	}
	if ( hdr->a_magic != A_MAGIC1 ) {
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
char *name;
int mode;
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
 *===============================
 * getodt - get ODT's attention
 *===============================
 * Input state - undefined -- nothing valid in memory
 * Output state - immediately after ODT prompt
 */
getodt()
{
	ioctl(DEVFD, TIOCSBRK, NULL);
	sleep(2);
	ioctl(DEVFD, TIOCCBRK, NULL);
	readuntil('@', 10);
	setmem(0, 5);		/* place a reset instruction at 0 */
	setmem(2, 0);		/* place a halt after it */
	setreg(LSIPSW, INTDISABLE);
	setreg(LSIPC, 0);
	sendodt("P", FALSE);	/* reset machine state	*/
	readuntil('@',10);
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
	char	lastch, input;

	settimer(time, "read timed out");
	if (setjmp(env)	!= 0) {
		fprintf(stderr, "readuntil did not find %s\n", unctrl(ch));
		callexit(1);
	}
	if ( A.verbose ) {
		fprintf(stderr,"IN: "); fflush(stderr);
	}
	input = ' ';
	do {
		lastch = input;
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
	if (ch == '@' && lastch == '^')
		readuntil(ch, time);
}

/*
 *=========================================
 * alarmhandler - return from alarm calls
 *=========================================
 */
alarmhandler()
{
	signal(SIGALRM, alarmhandler);
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
 *==============================================
 * sendodt - send a message to ODT half duplex
 *==============================================
 */
sendodt(msg, wait)
char *msg;
Bool wait;
{
	char *ptr;

	if ( A.verbose ) {
		fprintf(stderr,"OUT: ");
		for ( ptr=msg ; *ptr!=EOS ; ptr++ ) {
			fprintf(stderr,"%s",unctrl(*ptr));
		}
		fprintf(stderr,"\n");
	}
	while (*msg!=EOS) {
		write(DEVFD,msg,1);
                if ( wait )
                        readuntil(*msg, 10);
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
        if ( reg >= 0 )
                sprintf(buf,"R%d/",reg);
        else
                sprintf(buf,"$S/");
	sendodt(buf,TRUE);
        if ( !A.silent && !A.verbose )
                displayreg(reg);
	readuntil(' ',5);
	sprintf(buf,"%o\r",value&0xffff);
	sendodt(buf,TRUE);
	readuntil('@',5);
}
/*
 *==============================================
 * setmem - preload a mem location through ODT
 *==============================================
 */
setmem(loc, value)
int loc, value;
{
	char buf[32];

        sprintf(buf,"%o/",loc);
	sendodt(buf,TRUE);
	readuntil(' ',5);
	sprintf(buf,"%o\r",value&0xffff);
	sendodt(buf,TRUE);
	readuntil('@',5);
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
		fprintf(stderr,"%s\n",str);
	}
}

/*
 *=========================================
 * sendpack - send a packet to the LSI-11
 *=========================================
 */

#define putwsum(w) putw(w), sum += w
#define rawputc(c) putc(c,Fastline)

sendpack(addr, length, buf)
int addr, length;
lsiword *buf;
{
	int sum, i;

        if ( A.verbose ) {
                fprintf(stderr,"Sending packet addr=%6o len=%d\n",addr,length);
                for ( i=0 ; i<length ; i++ )
                        fprintf(stderr,"%03o ",buf[i]&0xff);
                fprintf(stderr,"\n");
        }
	rawputc(SOH);
	sum = 0;
	putwsum(addr);
	putwsum(length);
	for ( i=0 ; i<length ; i++ )
		putwsum(buf[i]);
        if ( A.verbose )
                fprintf(stderr,"sum is %d\n",(-sum)&0xffff);
        putwsum((-(lsiword)sum));
	fflush(Fastline);
}
/*
 *=============================================
 * putw - write a word to the high speed line
 *=============================================
 */
putw(word)
lsiword word;
{
	putch((char)(word&0xff));
	putch((char)((word>>8)&0xff));
}
/*
 *=================================================
 * putch - put a character on the high speed line
 *=================================================
 */
putch(ch)
char ch;
{
	if ( ch == ESC || ch == SOH )
		rawputc(ESC);
	rawputc(ch);
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
		fprintf(stderr,"\rPS     ");
        else
		fprintf(stderr,"\rR%d    ",val);
	fflush(stderr);
}
