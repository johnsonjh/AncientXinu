/*----------------------------------------------------------------------*/
/*									*/
/*	edl	--  Download to PDP-11 over Ethernet			*/
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
/*									*/
/*      Version Two: Dan Hachigian					*/
/*		     Doug Comer, eload (LSI side)			*/
/*	Modified to handle ethernet downloading, February, 1986		*/
/*	Modified to use new device database alloc. scheme, Spring 86	*/
/*	Significant structure reorganization, Spring 86			*/
/*									*/
/*	Version Three: Tom Stonecypher 10/86				*/
/*		Implemented stateless memory server protocol		*/
/*		Longjmp botches fixed, output format fixed, power-up	*/
/*		Initialization of memory-map unit fixed			*/
/*		Modified to allow download to exec this program		*/
/*		Structure reorganized: serial downloader modularized	*/
/*									*/
/* (c) Copyright, 1982, All rights reserved.				*/
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
#include "/usr/include/ctype.h"

/* 4.2 Network include files */
#include "/usr/include/netinet/in.h"
#include "/usr/include/sys/socket.h"
#include "/usr/include/netdb.h"


/*----------------------------------------------------------*/
/*    Include files - XINU domain                           */
/*----------------------------------------------------------*/
#include <a.out.h>
#include <getdev.h>
#include <download.h>
#include <dlpack.h>
#include <lsidl.h>
#include <ethdown.h>
#include "../eload/src/.version"
#include "../eload/src/.entry"

struct	arglist A;
jmp_buf	env;			/* used to return from read time-outs	*/
static	char *alarm_msg;	/* message printed when alarm signalled	*/
char	*Openfiles[MAXFILES];	/* open file names for error reporting	*/
char	*locknm;
extern	int errno;
int	Sfd;			/* socket descriptor for upd write/reads*/

/*
 * MAIN -- download user's program from host into LSI's memory
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	int	eloadfd, fMap, retry;
	u_long	lsi_ip;
	struct	exec ehdr, aouthdr;

	locknm = argv[--argc];
	machine = argv[--argc];
					/* cputype = get_cputype(machine); */
	procargs(argc, argv);		/* process arguments into struct A */
	initsigs();

	Openfiles[DEVFD] = get_name(machine);
	Openfiles[AOUTFD] = A.filename;

	getudp_port(&lsi_ip, machine);

	if (lsi_ip != 0) {		/* do an ethernet download */
	   retry = 0;
	   fMap = MAPMEM;
	   gethdr(AOUTFD, &aouthdr);
	   do {
		if ( A.reloadether || retry == 1 ) {
			if (!A.verbose && !A.silent)
				fprintf(stderr,"%sloading eload...\n",
						(A.reloadether?NULL:"re"));
			eloadfd = openfile(ELOAD, O_RDONLY);
			gethdr(eloadfd, &ehdr);
			serial_load(eloadfd, &ehdr, NOAUTOSTART);
			readuntil('@', 10);
			fMap = NOMAP;
		}
		if (ethload(&aouthdr, lsi_ip, fMap) == ETH_OK)
			break;
		lseek(AOUTFD, sizeof(aouthdr), 0);
	   } while (++retry == 1 && !A.reloadether);
	}
	if (lsi_ip == 0 || retry == 2 || (retry == 1 && A.reloadether)) {
		message("performing serial download...");
		serial_load(AOUTFD, &aouthdr, A.startdelay);
		initstart(&aouthdr);
	}
	touch(locknm);
	message("\rdone         ");
	callexit(0);
}

/*
 *====================================================================
 * procargs - parse and check command line, setup global A structure
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
 * output state: a.out in memory, executing if autostart else cpu halted
 */
serial_load(aout_fd, aout_hdr, delay)
	int	aout_fd;
	struct	exec *aout_hdr;
	int	delay;
{
	int ldrfd;
	struct exec ldr_hdr;

	/* place bootloader in memory */
	ldrfd = openfile(LSILOADER, O_RDONLY);
	gethdr(ldrfd, &ldr_hdr);
	getodt();
	setmem(SR0_ADDR, DISABLE_MMU);
	message("\rloading boot program...");
	bootload(ldrfd, &ldr_hdr, 0);
	close(ldrfd);
	start_ldr(aout_hdr, delay);
	slowload(aout_fd, aout_hdr);
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
	usleep(200000);
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

/*===============================================================
 * getserack - get an ack from serial downloader over serial line
 *===============================================================
*/
char getserack()
{
	char	resp;

	settimer(6, "slowload read timed out");
	if (setjmp(env) == EINTR)
		callexit(1);
	if ( read ( DEVFD, &resp, 1) != 1 ) {
		perror(Openfiles[DEVFD]);
		exit(1);
	}
	canceltimer();
	if (resp == NAK)
		fprintf(stderr,"NAK\n");
	else if (A.verbose && resp == ACK)
		fprintf(stderr, "ACK\n");
	else if (A.verbose)
		fprintf(stderr,"response is %s\n",unctrl(resp));
	return(resp);
}

/*============================================================
 * slowload - load user program via packets over serial line
 *============================================================
 * input  state: serial downloader running, ready to receive packets
 * output state: a.out file loaded into memory; a.out running if auto-
 *		 start, before odt prompt if no autostart
 */

FILE *Fastline;
char Fastbuf[BUFSIZ];

slowload(fd,ahdr)
int	fd;
struct exec *ahdr;
{
        int words, addr, length, total;
	FILE *infile;
	lsiword buf[PACKSIZE];
	char resp;

	words = (ahdr->a_text+ahdr->a_data) / sizeof (lsiword);
	Fastline = fdopen(DEVFD,"w");
	setbuf(Fastline,Fastbuf);

	infile = fdopen(fd,"r");
	fseek(infile, (long) sizeof (struct exec), 0);
	addr = LOADSTART;
	while ( words > 0 ) {
		length = (words>PACKSIZE ? PACKSIZE : words);
		fread(buf,sizeof(lsiword),length,infile);
		do
			sendserpack(addr,length,buf);
		while ( getserack() != ACK );

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
	        sendserpack ( addr, 0, buf );
	} while (getserack() != ACK);
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
 *============================================================================
 * getudp_port - get a udp port connected to the lsi port for packet transfer
 *============================================================================
 */
getudp_port(lsi_ip, machine)
u_long	*lsi_ip;
char	*machine;
{
	struct	sockaddr_in sin, sout;
	struct	hostent	*phost;
	char	*hostname;

	hostname = get_hname(machine);
	if (isdigit(*hostname))	/* IP dot notation instead of name */
		*lsi_ip = inet_addr(hostname);
	else {			/* hostname format */
		phost = gethostbyname(hostname);
		if ((char *)phost != NULL && phost->h_addrtype == AF_INET)
			bcopy(phost->h_addr, lsi_ip, sizeof lsi_ip);
		else
			*lsi_ip = 0;
	}
	if ((Sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("creating socket");
		callexit(1);
	}
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = 0;
	if (bind(Sfd, (caddr_t) &sin, sizeof(sin)) < 0) {
		perror("bind");
		callexit(1);
	}
	sout.sin_family = AF_INET;
	sout.sin_addr.s_addr = *lsi_ip;
	sout.sin_port = htons((u_short) DLUDP);

	if (connect(Sfd, &sout, sizeof(sout)) < 0) {
		perror("connect");
		callexit(1);
	}
}

/*============================================================================
 * start_eload - init lsi and start up eload (have memory mapped if requested)
 *============================================================================
 * input  state: after the odt prompt
 * output state: undefined (waiting for an ack from eload)
 */
start_eload(ip_addr, fMapMem)
u_long	ip_addr;
int	fMapMem;
{
	getodt();
	message("\rstarting resident loader...");
	setreg(PSW, INTDISABLE);
	if (fMapMem == MAPMEM)
		map_mem();/* phys 7 to virt 0, I/O to virt 7 */
	else
		load_args(ip_addr);
	setreg(SP, ETH_INITSP);
	setreg(PC, ETH_INITPC & (~1));
	sendodt("P", TRUE);
}

/*
 *===============================================================
 * load_args - put arguments to ELOAD into the appropriate locs
 *===============================================================
 */
load_args(lsi_ip)
u_long lsi_ip;
{
	setmem(0, lsi_ip & 0xffff);
	setmem(2, lsi_ip >> 16 & 0xffff);
}

/*
 *========================================================
 * map_mem - set up virtual memory before starting eload
 *========================================================
 */
map_mem()
{
	setmem(PAR0_ADDR, PAR0_VAL);	/* par0 -> physical 7 */
	setmem(PAR7_ADDR, PAR7_VAL);	/* par7 -> I/O space */
	setmem(PDR0_ADDR, PDR_VAL);	/* page len=8K, RW access */
	setmem(PDR7_ADDR, PDR_VAL);
	setmem(SR3_ADDR, 0);		/* 18-bit bus addresses	*/
	setmem(SR0_ADDR, ENABLE_MMU);
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
		fprintf( stderr,"%s: EOF reading a.out header\n",
			Openfiles[fd] );
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
	message("\rgetting odt...");
	ioctl(DEVFD, TIOCSBRK, NULL);
	usleep(300000);
	ioctl(DEVFD, TIOCCBRK, NULL);
	/* reset machine state -- necessary after power-up */
	readuntil('@',10);
	setmem(SR0_ADDR, DISABLE_MMU);
	setmem(0, 5);		/* place a reset instruction at 0 */
	setmem(2, 0);		/* place a halt after it */
	setreg(PSW, INTDISABLE);/* (reset disables MMU) */
	setreg(PC, 0);
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
	if (setjmp(env)	== EINTR) {
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
 *===========================================================
 * sendserpack - send a packet to the LSI-11 over serial line
 *===========================================================
 */

#define putwsum(w) putw(w), sum += w
#define rawputc(c) putc(c,Fastline)

sendserpack(addr, length, buf)
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
	fprintf(stderr,"\r%d         ",val);
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
		fprintf(stderr,"\rPS         ");
        else
		fprintf(stderr,"\rR%d        ",val);
	fflush(stderr);
}

struct	dl_pack	ackdl;

/*
 *=============================================
 * getack - get an ack from ELOAD over ethernet
 *=============================================
 */
struct	dl_pack	*getack()
{
	char	*ackmsg;

	settimer(ACKTIME, NULL);
	if (setjmp(env)	== EINTR)		/* timed out		*/
		return(NULL);
	if (read(Sfd, &ackdl, sizeof ackdl) < 0) {
		perror("udp packet read");
		callexit(1);
	}
	canceltimer();
	if (A.verbose) {
		switch (ackdl.dl_hdr.dl_op) {

		case DLACK:
			ackmsg = "ACK";
			break;
		case DLNAK:
			ackmsg = "NAK";
			break;
		case DLRAK:
			ackmsg = "ready-ack";
			break;
		case DLQAK:
			ackmsg = "quit-ack";
			break;
		default:
			ackmsg = "invalid ack command";
			break;
		}
		fprintf(stderr,"IN(getack): %s, addr = 0%o\n", ackmsg,
			ntohl(ackdl.dl_hdr.dl_addr));
	}
	return(&ackdl);
}

/*
 *===========================================
 * ethload - download program over ethernet
 *===========================================
 */
ethload(aouthdr, lsi_ip, fMapMem)
int lsi_ip, fMapMem;
struct exec *aouthdr;
{
	u_short	addr, len, i;
	int	aoutsize, iNak, version, bsslen;
	struct	dl_pack packet, *pDlAck;

	start_eload(lsi_ip, fMapMem);
	build_dlpack(DLID, 0, 0, &packet);
	usleep(400000);
	for (iNak = 0; iNak < ETHRETRY; iNak++ ) {
		if (iNak > 0)
			fprintf(stderr,"\rretrying...");
		send_dlpack(Sfd, &packet);
		pDlAck = getack();
		if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLRAK)
			break;
	}
	if (iNak == ETHRETRY) {
		message("can't start eload");
		return(ETH_FAILED);
	}
	if ((version = pDlAck->dl_hdr.dl_vers & LOWBYTE) != VERSION &&
		version != ROMVERS) {
		message("version mismatch");
		return(ETH_FAILED);
	}
	aoutsize = aouthdr->a_text + aouthdr->a_data;
	addr = LOADSTART;
	while (aoutsize > 0) {
		len = aoutsize > DLMAXD ? DLMAXD : aoutsize;
		build_dlpack(DLDEP, len, addr, &packet);
		for (iNak = 0; iNak < ETHRETRY; iNak++ ) {
			if (iNak > 0)
				fprintf(stderr,"\rretrying...");
			send_dlpack(Sfd, &packet);
			pDlAck = getack();
			/* test for ACK & correct address sequence number */
			if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLACK &&
			    ntohl(pDlAck->dl_hdr.dl_addr) == addr)
				break;
		}
		if (iNak == ETHRETRY) {
			message("ethernet download failed");
			return(ETH_FAILED);
		}
		if (!A.silent && !A.verbose)
			if ((addr % 020000) == 0 ) {
				fprintf(stderr,
					"\rpage %d       ",
					addr/020000);
					fflush(stderr);
			}
		addr += len;
		aoutsize -= len;
	}

	/* now zero out the bss */

	bsslen = aouthdr->a_bss;
	for (i = 0; i <= DLMAXD; i++)
		packet.dl_pt.dl_data[i] = '\0';
	packet.dl_hdr.dl_vers = VERSION;
	packet.dl_hdr.dl_op = DLDEP;
	while (bsslen > 0) {
		len = bsslen > DLMAXD ? DLMAXD : bsslen;
		packet.dl_hdr.dl_len = htons(len);
		packet.dl_hdr.dl_addr = htonl(addr);
		for (iNak = 0; iNak < ETHRETRY; iNak++ ) {
			if (iNak > 0)
				fprintf(stderr,"\rretrying...");
			send_dlpack(Sfd, &packet);
			pDlAck = getack();
			/* test for ACK & correct address sequence number */
			if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLACK &&
			    ntohl(pDlAck->dl_hdr.dl_addr) == addr)
				break;
		}
		if (iNak == ETHRETRY) {
			message("ethernet download failed");
			return(ETH_FAILED);
		}
		if (!A.silent && !A.verbose)
			if ((addr % 020000) == 0 ) {
				fprintf(stderr,
					"\rpage %d   ",
					addr/020000);
					fflush(stderr);
			}
		addr += len;
		bsslen -= len;
	}
	/* now instruct eload to perform specified autostart option */
	packet.dl_pt.dl_srt.dl_entry = htonl(aouthdr->a_entry &(~1));
	packet.dl_pt.dl_srt.dl_delay = htonl(A.startdelay);
	build_dlpack(DLSRT, 0, 0, &packet);
	for ( iNak = 0; iNak < ETHRETRY; iNak++ ) {
		if (iNak > 0)
			fprintf(stderr,"\rretrying...");
		send_dlpack(Sfd, &packet);
		pDlAck = getack();
		if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLQAK)
			break;
	}
	return(iNak < ETHRETRY ? ETH_OK : ETH_FAILED);
}

/*
 *=============================================================
 * build_dlpack - construct a packet in ether download format
 *=============================================================
 */
build_dlpack(op, datalen, addr, packet)
char	 op;
unsigned short	 datalen;
unsigned long	 addr;
struct	 dl_pack *packet;
{
	int	i;

	packet->dl_hdr.dl_op = op;
	packet->dl_hdr.dl_vers = VERSION;

	switch (op) {

	case DLDEP:
		packet->dl_hdr.dl_len = htons(datalen);
		packet->dl_hdr.dl_addr = htonl(addr);
		if (datalen > 0)
		  if ( read(AOUTFD, packet->dl_pt.dl_data, datalen)
		      < datalen ) {
			fprintf(stderr, "read error\n");
			callexit(1);
		  }
		break;

	case DLSRT:
	case DLID:
		break;

	default:
		fprintf(stderr, "build_dlpack: invalid output packet type\n");
		break;
	}
}

/*
 *============================================================
 * send_dlpack - send a downloader format packet out via UDP
 *============================================================
 */
send_dlpack(sockfd, dlpacket)
int sockfd;
struct dl_pack *dlpacket;
{
	unsigned short	len;
	unsigned long	addr;
	unsigned int	pktsize;

	switch (dlpacket->dl_hdr.dl_op) {

	case DLDEP:
		pktsize = sizeof(struct dl_pack);
		break;

	case DLSRT:
		pktsize = sizeof(struct dl_header)
					+ sizeof(struct dl_srtval);
		break;

	default:
		pktsize = sizeof(struct dl_header);
		break;
	}
	if (A.verbose) {
		fprintf(stderr,"packet:  ");
		switch(dlpacket->dl_hdr.dl_op) {
		case DLDEP:
			addr =  ntohl(dlpacket->dl_hdr.dl_addr);
			len  =  ntohs(dlpacket->dl_hdr.dl_len);
			fprintf(stderr, "deposit, addr = %#o, len = %#o\n",
				addr, (int) len);
			break;
		case DLSRT:
			fprintf( stderr,
				"start program, delay = %d, entry pt = %o\n",
				ntohl(dlpacket->dl_pt.dl_srt.dl_delay),
				ntohl(dlpacket->dl_pt.dl_srt.dl_entry) );
			break;
		case DLID:
			fprintf(stderr, "ID packet\n");
			break;
		default:
			fprintf(stderr, "send_dlpack: invalid packet type\n");
			return;
		}
	}
	if (send(sockfd, dlpacket, pktsize, 0) < 0) {
		perror("udp packet send");
		callexit(1);
	}
}
