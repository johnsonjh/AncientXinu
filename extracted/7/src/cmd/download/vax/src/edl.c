/*----------------------------------------------------------------------*/
/*									*/
/*	edl	--  Download to VAX over Ethernet			*/
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

/* 4.3 Network include files						*/
#include "/usr/include/netinet/in.h"
#include "/usr/include/sys/socket.h"
#include "/usr/include/netdb.h"

/*    Include files - XINU domain                           		*/
#include <getdev.h>
#include <download.h>
#include <dlpack.h>
#include <vaxdl.h>
#include <ethdown.h>
#include "../eload/src/.version"

struct	arglist A;
jmp_buf	env;			/* used to return from read time-outs	*/
static	char *alarm_msg;	/* message printed when alarm signalled	*/
char	*Openfiles[MAXFILES];	/* open file names for error reporting	*/
char	*locknm;		/* name of lock file			*/
extern	int errno;
int	Sfd;			/* socket descriptor for upd write/reads*/

/*
 * MAIN -- download user's program from host into VAX's memory
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	int	eloadfd, retry;
	u_long	ip_addr;
	struct	exec ehdr, aouthdr;

	locknm = argv[--argc];
	machine = argv[--argc];
					/* cputype = get_cputype(machine); */
	procargs(argc, argv);		/* process arguments into struct A */
	initsigs();

	Openfiles[DEVFD] = get_name(machine);
	Openfiles[AOUTFD] = A.filename;

	getudp_port(&ip_addr, machine);
	gethdr(AOUTFD, &aouthdr, TRUE);

	if (ip_addr != 0) {		/* do an ethernet download */
	   retry = 0;
	   do {
		lseek(AOUTFD, N_TXTOFF(aouthdr), 0);
		if ( A.reloadether || retry == 1 ) {
			if (!A.verbose && !A.silent)
				fprintf(stderr,"%sloading eload...\n",
						(A.reloadether?NULL:"re"));
			eloadfd = openfile(ELOAD, O_RDONLY);
			gethdr(eloadfd, &ehdr, TRUE);
			initcpu();
			load_args(ip_addr);
			serial_load(eloadfd, ETH_INITPC, &ehdr, NOAUTOSTART);
		}
		if (ethload(&aouthdr, ip_addr, &ehdr) == ETH_OK)
			break;
	   } while (++retry == 1 && !A.reloadether);
	}
	if (ip_addr == 0 || retry == 2 || (retry == 1 && A.reloadether)) {
		message("performing serial download...");
		serial_load(AOUTFD, LOADSTART, &aouthdr, A.startdelay);
	}
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
 *=================================================
 * getudp_port - get a udp port for packet transfer
 *=================================================
 */
getudp_port(ip_addr, machine)
u_long	*ip_addr;
char	*machine;
{
	struct	sockaddr_in sin, sout;
	struct	hostent	*phost;
	char	*hostname;

	hostname = get_hname(machine);
	if (isdigit(*hostname))	/* IP dot notation instead of name */
		*ip_addr = inet_addr(hostname);
	else {			/* hostname format */
		phost = gethostbyname(hostname);
		if ((char *)phost != NULL && phost->h_addrtype == AF_INET)
			bcopy(phost->h_addr, ip_addr, sizeof ip_addr);
		else
			*ip_addr = 0;
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
	sout.sin_addr.s_addr = *ip_addr;
	sout.sin_port = htons((u_short) DLUDP);

	if (connect(Sfd, &sout, sizeof(sout)) < 0) {
		perror("connect");
		callexit(1);
	}
}

/*==========================================
 * start_eload - init lsi and start up eload
 *==========================================
 * input  state: after the console prompt
 * output state: undefined (waiting for an ack from eload)
 * actions:
 *    set PC to address of eload
 *    disable interrupts
 *    give download IP address of target VAX as arg to ELOAD
 *    start LSI
 */
start_eload()
{
	char	buf[32];

	initcpu();
	setreg(KSPREG, ETH_INITSP);
	sprintf(buf, "S %x\r", ETH_INITPC);
	sendcons(buf, TRUE);
}

/*
 *============================================================
 * load_args - put arguments to ELOAD into the appropriate loc
 *============================================================
 */
load_args(ip_addr)
u_long	ip_addr;
{
	setmem(ETH_INITPC-4, ip_addr);	
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
		fprintf(stderr,"IN(getack): %s, addr = 0x%x\n", ackmsg,
			ntohl(ackdl.dl_hdr.dl_addr));
	}
	return(&ackdl);
}

/*
 *===========================================
 * ethload - download program over ethernet
 *===========================================
 */
ethload(aouthdr, ip_addr, ehdr)
int ip_addr;
struct exec *aouthdr, *ehdr;
{
	u_long	addr;
	u_short	len, i;
	int	aoutsize, iNak, version;
	struct	dl_pack packet, *pDlAck;

	message("starting resident loader");
	start_eload();
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
	if ( (version = pDlAck->dl_hdr.dl_vers & LOWBYTE) != VERSION &&
		version != ROMVERS ) {
		message("version mismatch");
		return(ETH_FAILED);
	}
	aoutsize = aouthdr->a_text + aouthdr->a_data;
	addr = LOADSTART;
	while (aoutsize > 0) {
		if (addr >= ETH_INITPC) {
			fprintf(stderr,"a.out image too large\n");
			callexit(1);
		}
		len = aoutsize > DLMAXD ? DLMAXD : aoutsize;
		build_dlpack(DLDEP, len, addr, &packet);
		for ( iNak = 0; iNak < ETHRETRY; iNak++ ) {
			if (iNak > 0)
				fprintf(stderr,"\rretrying...");
			send_dlpack(Sfd, &packet);
			pDlAck = getack();
			/* test for ACK & correct address sequence number */
			if ( pDlAck != NULL &&
			     pDlAck->dl_hdr.dl_op == DLACK &&
			     ntohl(pDlAck->dl_hdr.dl_addr) == addr )
				break;
		}
		if (iNak == ETHRETRY) {
			message("ethernet download failed");
			return(ETH_FAILED);
		}
		if (!A.silent && !A.verbose)
			if ( (addr % 0x2000) == 0 ) {
				fprintf(stderr,
					"\rpage %d       ",
					addr/0x200);
			}
		addr += len;
		aoutsize -= len;
	}
	packet.dl_pt.dl_srt.dl_entry = htonl(aouthdr->a_entry &(~1));
	packet.dl_pt.dl_srt.dl_delay = htonl(A.startdelay);
	build_dlpack(DLSRT, 0, 0, &packet);
	for ( iNak = 0; iNak < ETHRETRY; iNak++ ) {
		if (iNak > 0)
			fprintf(stderr,"\rretrying...");
		send_dlpack(Sfd, &packet);
		pDlAck = getack();
		if ( pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLQAK )
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
			fprintf(stderr, "deposit, addr = 0x%#x,\
				len = 0x%#x\n",	addr, (int) len);
			break;
		case DLSRT:
			fprintf( stderr,
				"start program, delay = %d, entry = 0x%x\n",
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
