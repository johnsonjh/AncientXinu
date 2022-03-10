/*----------------------------------------------------------------------*/
/*									*/
/*	eul	--  Upload from VAX to UNIX machine over ethernet	*/
/*									*/
/* This program uploads a memory image from a VAX into a coreX file	*/
/* on a UNIX machine.							*/
/*									*/
/* Configuration -							*/
/*	VAX tty line connected to VAX console port			*/
/*									*/
/* Usage:								*/
/*	upload [-lx] [-o filename] [ [minaddr] max addr ]		*/
/*									*/
/* Author:								*/
/*		Tom Stonecypher						*/
/*		Purdue University CS Department				*/
/*		December, 1986						*/
/*		based on the LSI 11 ethernet uploader			*/
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

/* 4.3 Network include files */
#include "/usr/include/netinet/in.h"
#include "/usr/include/sys/socket.h"
#include "/usr/include/netdb.h"

/* Xinu domain include files */
#include <coreX.h>
#include <baud.h>
#include <getdev.h>
#include <ethdown.h>
#include <vaxul.h>
#include <dlpack.h>
#include <upload.h>
#include "../eload/src/.version"

char 	*msgval;
char	*Openfiles[20];
struct	coreX	Corehead;
jmp_buf	env;			/* environment to return to on timeout	*/
static	char *alarm_msg;	/* message printed when alarm signalled	*/
struct	arglist	A;		/* parsed argument values		*/
char	*lockfile;		/* path name of lockfile		*/
struct	sgttyb	VAXttyb;
char	*index();
int	touch();
extern	int	errno;
int	Sfd;			/* socket descriptor for udp read/writes*/

/*
 * MAIN -- upload memory image from VAX to UNIX machine
 */
main ( argc, argv )
int argc;
char *argv[];
{
	char	*machine, *cputype;
	u_long	ip_addr;
	int	i, core_fd;

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

	Corehead.c_zero1 = Corehead.c_zero2 = 0;
	Corehead.c_zero3 = Corehead.c_zero4 = Corehead.c_zero5 = 0;
	Corehead.c_magic = COREMAGIC;
	Corehead.c_size = 0;	/* size currently not used */

	getudp_port(&ip_addr, machine);

	if (ip_addr == 0 || ethuload(ip_addr) != ETH_OK ) {
		/* can't do ethernet upload */
		message("performing serial upload...\n",0,0);
/* patch here */
	}
	touch(lockfile);
	message("\rdone         \n",0,0);
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

/*
 *======================================================================
 * procargs - parse and check arguments, modifying global A structure
 *======================================================================
 */
procargs(argc, argv)
int argc;
char *argv[];
{
	int	arg, i, unswitched;
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
				fprintf(stderr,"-a arg. not implemented\n");
				break;
			default:	/* generic uploader already parsed: */
				break;	/* don't complain about arguments */
			}
	else /* there's no dash in front */
		switch ( unswitched++ )	{
		case 0:
			A.highaddr = atoi(argv[arg]);
			message("highaddr=%x\n",A.highaddr,0);
			break;
		case 1:
			A.lowaddr = A.highaddr;
			A.highaddr = atoi(argv[arg]);
			message("lowaddr=%x\n",A.lowaddr,0);
			message("highaddr=%x\n",A.highaddr,0);
			break;
		default:
			break;		/* don't complain -- already parsed */
		}
	}
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
 *==================================================
 * getipl - get	the current interrupt priority level
 *==================================================
 */
getipl(ipl)
int	*ipl;
{
	char buf[60];

	sendcons("E/I 12\r");
	readuntil("\r", buf, 7, 32);
	sscanf(buf, "%*s %*x %x", ipl);
}

/*
 *==========================================
 * getregs - get contents of the registers
 *==========================================
 */
getregs(regs)
int	*regs;
{
	int r;
	char buf[64], *val;

	for ( r=0 ; r<REGISTERS; r++ )	{
		if (r < R0) {
			sprintf(buf, "E/I %x\r", r);
			sendcons(buf, TRUE);
		}
		else if (r == R0)
			sendcons("E/L/G 0\r", TRUE);
		else if (r <= FP)
			sendcons("E +\r", TRUE);
		else if (r == PC)
			sendcons("E/L/G F\r", TRUE);
		else if (r == PSL)	/* best can do is get IPL */
			sendcons("E/I 12\r", TRUE);
		else {			/* p0br, p0lr, p1br, p1lr */
			sprintf(buf, "E/I %x\r", r-12);
			sendcons(buf, TRUE);
		}
		readuntil("\r", buf, 7, 32);
		sscanf(buf,"%*s %*x %x", regs);
		if (r == PSL)
			*regs = *regs << 16;
		regs++;
		if ( !A.silent && !A.verbose )
			displayval(r);
	}
	if ( !A.silent && !A.verbose )
		fprintf(stderr,"\r");
	getprompt(6);
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
		case DLENK:
			ackmsg = "upload out of bounds";
			break;
		default:
			ackmsg = "invalid ack command";
			break;
		}
		fprintf( stderr,
			"IN(getack): %s, addr = 0x%x, len = 0x%x\n",
			ackmsg, ntohl(ackdl.dl_hdr.dl_addr),
			ntohs(ackdl.dl_hdr.dl_len) );
	}
	return(&ackdl);
}

/*
 *=============================================
 * ethuload - upload memory image over ethernet
 *=============================================
 */
ethuload(ip_addr)
int ip_addr;
{
	u_long	addr;
	u_short	len;
	int	iNak,	version;
	struct	dl_pack packet,	*pDlAck;

	start_eload();
	build_dlpack(DLID, 0, 0, &packet);
	usleep(400000);
	for (iNak = 0; iNak < ETHRETRY; iNak++ ) {
		if (iNak > 0)
			fprintf(stderr,"\rretrying...");
		send_ulpack(Sfd, &packet);
		pDlAck = getack();
		if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLRAK)
			break;
	}
	if (iNak == ETHRETRY) {
		message("can't start eload\n",0,0);
		return(ETH_FAILED);
	}
	if ( (version = pDlAck->dl_hdr.dl_vers & LOWBYTE) != VERSION &&
	     version != ROMVERS && !A.silent )
		fprintf(stderr,"version mismatch, trying upload anyway\n");
	for ( addr = A.lowaddr ; addr <= A.highaddr ; addr += len ) {
		len = addr+DLMAXD > A.highaddr ? A.highaddr-addr+1 : DLMAXD;
		build_dlpack(DLEXT, len, addr, &packet);
		for (iNak = 0; iNak < ETHRETRY; iNak++ ) {
			if (iNak > 0)
				fprintf(stderr,"\rretrying...");
			send_ulpack(Sfd, &packet);
			pDlAck = getack();
			/* test for ACK & correct address sequence number */
			if (pDlAck != NULL)
			if ( pDlAck->dl_hdr.dl_op == DLEAK &&
			     ntohl(pDlAck->dl_hdr.dl_addr) == addr )
				break;
			else if (pDlAck->dl_hdr.dl_op == DLENK)
				break;	/* done uploading */
		}
		if (pDlAck->dl_hdr.dl_op == DLENK) {
			A.highaddr = addr - 1;
			break;
		}
		if (iNak == ETHRETRY) {
			message("ethernet upload failed\n",0,0);
			return(ETH_FAILED);
		}
		if (!A.silent && !A.verbose)
			if ( (addr % 0x2000) == 0 ) {
				fprintf(stderr,
					"\rpage %d       ",
					addr/0x200);
			}
		/* copy uploaded memory into place */
		len = (int) ntohs(pDlAck->dl_hdr.dl_len);
		if ( write(COREFD, pDlAck->dl_pt.dl_data, len) != len ) {
			perror(Openfiles[COREFD]);
			callexit(1);
		}
	}
	/* instruct eload to halt VAX */
	packet.dl_pt.dl_srt.dl_entry = 0; /* dummy value */
	packet.dl_pt.dl_srt.dl_delay = htonl(NOAUTOSTART);
	build_dlpack(DLSRT, 0, 0, &packet);
	for ( iNak = 0; iNak < ETHRETRY; iNak++ ) {
		send_ulpack(Sfd, &packet);
		pDlAck = getack();
		if (pDlAck != NULL && pDlAck->dl_hdr.dl_op == DLQAK)
			break;
	}
	/* don't complain if got no ACK */
	return(ETH_OK);
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
	packet->dl_hdr.dl_op = op;
	packet->dl_hdr.dl_vers = VERSION;

	switch (op) {
	case DLEXT:
		packet->dl_hdr.dl_len = htons(datalen);
		packet->dl_hdr.dl_addr = htonl(addr);
		break;
	case DLID:			/* do nothing, don't complain */
	case DLSRT:
		break;
	default:
		fprintf(stderr, "build_dlpack: invalid output packet type\n");
		break;
	}
}

/*
 *============================================================
 * send_ulpack - send a downloader format packet out via UDP
 *============================================================
 */
send_ulpack(sockfd, dlpacket)
int sockfd;
struct dl_pack *dlpacket;
{
	unsigned short	len;
	unsigned long	addr;
	unsigned int	pktsize;

	switch (dlpacket->dl_hdr.dl_op) {
	case DLSRT:
		pktsize = sizeof(struct dl_header) +
					sizeof(struct dl_srtval);
		break;
	case DLEXT:
	case DLID:
	default:
		pktsize = sizeof(struct dl_header);
		break;
	}
	if (A.verbose) {
		fprintf(stderr,"packet:  ");
		switch(dlpacket->dl_hdr.dl_op) {

		case DLEXT:
			addr =  ntohl(dlpacket->dl_hdr.dl_addr);
			len  =  ntohs(dlpacket->dl_hdr.dl_len);
			fprintf(stderr, "extract, addr = %#x, len = %#x\n",
				addr, (int) len);
			break;
		case DLSRT:
			fprintf( stderr,
				"start program, delay = %d, entry pt = %x\n",
				ntohl(dlpacket->dl_pt.dl_srt.dl_delay),
				ntohl(dlpacket->dl_pt.dl_srt.dl_entry) );
			break;
		case DLID:
			fprintf(stderr, "ID packet\n");
			break;
		default:
			fprintf(stderr, "send_ulpack: invalid packet type\n");
			return;
		}
	}
	if (send(sockfd, dlpacket, pktsize, 0) < 0) {
		perror("udp packet send");
		callexit(1);
	}
}

/*====================================================
 * savestate - save initial registers before uploading
 *====================================================
*/
savestate()
{
	getcpu();
	message("getting registers...\n",0,0);
	getregs(Corehead.c_regs);
	getipl(&Corehead.c_psw);
	lseek(COREFD, 0, 0);
	write(COREFD, &Corehead, sizeof Corehead);
}

/*==========================================
 * start_eload - init vax and start up eload
 *==========================================
 * input  state: after the console prompt
 * output state: undefined (waiting for an ack from eload)
 * actions:
 *    set PC to address of eload
 *    disable interrupts
 *    start VAX
 */
start_eload()
{
	char	buf[32];

	savestate();
	sendcons(CPUINIT, TRUE);
	message("starting VAX-resident portion of loader...\n",0,0);
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
 *==================================================
 * sendcons - send a message to console half duplex
 *==================================================
 */
sendcons(msg, wait)
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
		if (islower(*msg))
			*msg = *msg - 32;
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
	int	i;
	char	buf[32];

	for (i=0; i<3; i++)
		readuntil (">", buf, time, 32);
}
	
/*
 *======================================================
 * getcpu - get VAX console's attention, initialize cpu
 *======================================================
 * Input state - undefined
 * Output state - immediately after VAX console prompt
 */
getcpu()
{
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
