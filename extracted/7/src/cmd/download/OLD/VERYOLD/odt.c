#include <stdio.h>
#include <sgtty.h>
#include <signal.h>
#include <ctype.h>

#define	TTYLINE	"/dev/LSI"		/* default LSI 11 tty line	*/

/*----------------------------------------------------------------------*/
/*									*/
/*  odt - asynchronous I/O between two tty ports                        */
/*									*/
/*  synopsis:								*/
/*      odt [ttyname]                                                   */
/*									*/
/*  description:							*/
/*      odt attach two tty ports together.  Stdin and                   */
/*	stdout constitute port A, "ttyname" constitutes port B.		*/
/*	Input from A is sent to B, input from B is sent to A.		*/
/*	Input from A is processed as follows:  lines beginning with	*/
/*      ! are passed to a subshell.  A \ character causes odt           */
/*	change they keyboard mode to RAW for the next character.	*/
/*	Input from A is echoed to A's output.				*/
/*									*/
/* Author/status:							*/
/*      Bob Brown/CS Dept/Purdue University				*/
/*	Doug Comer							*/
/*									*/
/* Example of use:							*/
/*	sleep 10000 >/dev/tty12 &					*/
/*	stty 1200 >/dev/tty12						*/
/*      odt /dev/tty12                                                  */
/*                                                                      */
/*----------------------------------------------------------------------*/

#define STDOUT	1
#define STDIN	0

int istty;
int pid, ppid;
int line;
int lflags, flags;
long starting;

struct sgttyb ltty, tty, linetty;

int onsusp(), onresum();
extern int quit();
extern int errno;
extern char *malloc();
extern char *index(), *ctime();
/*
 *========================
 *   M a i n   E n t r y
 *========================
 */
main(argc, argv)
int argc;
char *argv[];
{

	int tries, opened, i;
	signal(SIGINT, quit);
        if ( argc>=2 ) {
		if ((line=open(argv[1],2)) < 0 ) {
			perror(argv[1]);
			exit(1);
		}
	} else {
		/*lock();*/
		if ((line=open(TTYLINE,2)) < 0 ) {
			perror(TTYLINE);
			exit(1);
		}
	}
	gtty(line,&linetty);
	linetty.sg_flags |= RAW;
	linetty.sg_flags &= ~ECHO;
	linetty.sg_ospeed = linetty.sg_ispeed = B9600;
	stty(line,&linetty);
	if (line < 0) {
		perror(argv[1]);
		exit(1);
	}
	/*
         * set up local terminal in cbreak mode
	 */
	if(ioctl(0,TIOCGETP,&tty) >= 0 ) {
		istty = 1;
		flags = tty.sg_flags;
		tty.sg_flags |= CBREAK;
                tty.sg_flags &= ~ECHO;
                tty.sg_flags &= ~CRMOD;
		if(ioctl(0,TIOCSETP,&tty)==-1) {
			perror("ioctl SET");
		}
	}
	else
		istty = 0;
	/*
	 * start up the keyboard monitor and line monitor tasks
	 */
	ppid = getpid();
	if ((pid=fork()))
		keymon();
	else
		linemon();
	exit(0);
}
/*
 *------------------------------------------------------------------
 *
 * keymon() - terminal keyboard monitor
 *
 *------------------------------------------------------------------
 */
keymon()
{
	int col, i;
        int esc;
        char c, buf[BUFSIZ];
	/*
	 * loop forever over the following: process each keystroke
	 * on the keyboard.
	 */
	signal(SIGTSTP, onsusp);
	signal(SIGCONT, onresum);
	col = 0;
        esc = 0;
	while( (i=read(0,&c,1)) != 0) {
	    if (i == 1) {
		if ( col==0 && c == '!' ) {
                        while ( c != '\n' && c != '\r' ) {
				read(0,&c,1);
                                buf[col++] = c;
                        }
                        buf[--col] = '\0';
			ex(buf);
			col = 0;
		} else if ( c=='\\' && esc==0 ) {
			tty.sg_flags |= RAW;
			stty(0,&tty);
                        esc = 1;
		} else {
                        if ( esc && (c&0177)=='\0' ) {
                                ioctl(line,TIOCSBRK,NULL);
                                sleep(2);
                                ioctl(line,TIOCCBRK,NULL);
                        } else {
                                write(line, &c, 1);
                                if ( (c&0177)=='\n' || (c&0177)=='\r' )
                                        col =  0;
                                else
                                        col++;
                        }
                        if ( esc ) {
                                tty.sg_flags &= ~RAW;
                                stty(0,&tty);
                        }
                        esc = 0;
		}
	    }
	}
        esc = 0;
	if ( istty ) {
		tty.sg_flags = flags;
		ioctl(0,TIOCSETP,&tty);
	}
}

quit()
{
	FILE *fd;
	long ending;
	char *asc;
	char tmp[100];
	kill(pid,9);
	if ( istty ) {
		tty.sg_flags = flags;
		ioctl(0,TIOCSETP,&tty);
		printf("\n");
	}
	exit(0);
}

linemon()
{
	char c;
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP,SIG_DFL);
	signal(SIGCONT, SIG_DFL);
	for (;;) {
		if (read( line, &c, 1) != -1)
			write(STDOUT, &c, 1);
	}
}
ex(bp)
char *bp;
{
	register i;
	int saved;
	struct sgttyb tio;
	int (*oldint)(), (*oldquit)();

	ioctl(STDIN, TIOCGETP, &tio);
	saved = tio.sg_flags;
	if ( istty ) {
		tio.sg_flags = flags;
		ioctl(STDIN,TIOCSETP,&tio);
	}
	oldint = signal(SIGINT, SIG_DFL);
	oldquit = signal ( SIGQUIT, SIG_DFL);

	system(bp,1);
	signal ( SIGINT, oldint);
	signal ( SIGQUIT, oldquit);
	if ( istty ) {
		tio.sg_flags = saved;
		ioctl(STDIN, TIOCSETP, &tio);
	}
	printf("!\n");
}
system(s,uidsw)
char *s;
int uidsw;
{
	int status, pid, w;
	register int (*istat)(), (*qstat)();

	if ((pid = vfork()) == 0) {
		if ( uidsw )
			setuid(getuid());
		execl("/bin/sh", "sh", "-c", s, 0);
		_exit(127);
	}
	istat = signal(SIGINT, SIG_IGN);
	qstat = signal(SIGQUIT, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, istat);
	signal(SIGQUIT, qstat);
	return(status);
}


#define LOCKFILE "/tmp/LSIlock"
#define RESPGM	"/usr/Xinu/bin/LSIunlock"

/*
 *====================================================
 * lock - prevent simultaneous access to LSI-11 line
 *====================================================
 */
lock()
{
        int lf, len;
        char buf[100], name[100];

        /*
         * if lock file exists - someone is using the line (maybe me)
         */
        if ((lf=creat(LOCKFILE,0444)) < 0 ) {
                if ((lf=open(LOCKFILE,0)) < 0 ) {
                        perror(LOCKFILE);
                        exit(1);
                }
                buf[read(lf,buf,100)] = '\0';
                sscanf(buf,"%s",name);
                if ( strcmp(name,getlogin()) == 0 )
                        return;
                fprintf(stderr,"LSI in use by %s\n",buf);
                exit(1);
        }
        /*
         * not in use - reserve and start timer
         */
        sprintf(buf,"%s on %s",getlogin(), ttyname());
        write(lf,buf,strlen(buf));
        close(lf);
        if ( fork() == 0 ) {
                signal(SIGHUP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
                signal(SIGTERM, SIG_IGN);
                signal(SIGQUIT, SIG_IGN);
                sprintf(buf,"LSI in use by %s on %s for",getlogin(),ttyname());
                execl(RESPGM,buf,"10","minutes");
                perror(RESPGM);
                exit(1);
        }
}
struct sgttyb savetty, newtty;
onsusp()
{
        ioctl(0,TIOCGETP,&savetty);
        newtty = savetty;
        newtty.sg_flags = flags;
        ioctl(0,TIOCSETP,&newtty);
        fflush(stdout);
	signal(SIGTSTP, SIG_DFL);
	kill(getpid(), SIGTSTP);
}
onresum()
{
	signal(SIGTSTP, onsusp);
	ioctl(0,TIOCSETP,&savetty);
}
