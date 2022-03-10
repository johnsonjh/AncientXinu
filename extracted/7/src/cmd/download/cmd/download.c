/* download.c -- download */

#include "/usr/include/stdio.h"
#include "/usr/include/sgtty.h"
#include "/usr/include/ctype.h"
#include <baud.h>
#include <getdev.h>
#include "/usr/include/sys/file.h"
#include <download.h>

static	struct arglist A;
static	char execfile[MAXBIN] = {LIBPATH};

/*==============================================
 * download -- outer level of generic downloader
 *==============================================
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	DevDesc	*Dev;
	int	aout_fd, ndbargs, i, j;
	char	*argvec[MAXARGS];

	initargs();		/* initialize A argument structure */
	procargs(argv, argc);	/* process arguments into struct A */
	Dev = getdev(A.class, A.machnum, A.devflags);
	if ((char *)Dev == NULL)
		exit(1);
	ndbargs = getdbargs(Dev, argvec); /* get argument field from db */
	argvec[0] = "db";		/* in case error in argument */
	procargs(argvec, ndbargs);
	using_msg(Dev, argv[0]);
	linemode(Dev->fd, A.baudrate, RAW, Dev);
	aout_fd = openfile(A.filename, O_RDONLY);
	dup2(aout_fd, AOUTFD);	/* make the a.out file the standard input */
	dup2(Dev->fd, DEVFD);	/* make the device the standard output    */
	for ( i=ndbargs+1, j=1 ; j < argc && i < MAXARGS-3 ; )
		argvec[i++] = argv[j++];
	argvec[i++] = Dev->machine;
	argvec[i++] = Dev->locknm;
	if (i == MAXARGS) {
		fprintf(stderr,"%s: too many arguments\n", argv[0]);
		exit(1);
	}
	argvec[i] = NULL;
	strncat(execfile, ((A.downloader == NULL) ? get_download(Dev->machine)
			: A.downloader), MAXBIN - strlen(execfile) - 1);
	argvec[0] = execfile;
	if (A.verbose)
		fprintf(stderr,"exec binary %s\n",execfile);
	execv(execfile, argvec); /* run the downloader specified in db */
	perror(execfile);	/* execv failed */
}
	
/*
 *======================================================================
 * procargs - parse and check command line, modifying global A structure
 *======================================================================
 */
procargs(argv, argc)
char *argv[];
int argc;
{
	int arg, i;
	int unswitched;

	unswitched = 0;
	for ( arg=1 ; arg < argc ; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			switch ( argv[arg][1] ) {
			case 'c':
				A.class = argv[arg]+2;
				break;
			case 'l':
				for (i=2; argv[arg][i] != '\0'; i++)
					if (!isdigit(argv[arg][i]&LOWBYTE))
						usagexit(argv[0]);
				A.machnum=atoi(argv[arg]+2);
				break;
			case 'B':
				if (*(argv[arg]+2) == '\0') {
					A.baudrate = NOBAUD;
					break;
				}
				for (i = 0; i < NBAUD; ++i)
					if (strcmp(baudlist[i].name,
						argv[arg]+2) == 0)
							break;
				if (i < NBAUD)
					A.baudrate = baudlist[i].rate;
				else {
					fprintf(stderr,
						"Unknown baud rate: %s\n",
						argv[arg]+1);
					exit(1);
				}
				break;
			case 'h':
				A.devflags |= GD_HLDDEVS;
				break;
			case 'e':
				A.reloadether = TRUE;
				break;
			case 'd':
				A.downloader = argv[arg]+2;
				break;
			case 'v':
				A.verbose = TRUE;
				break;
			case 's':
				A.silent = TRUE;
				break;
			case 'a':
				for (i=2; argv[arg][i] != '\0'; i++)
					if (!isdigit(argv[arg][i]&LOWBYTE))
						usagexit(argv[0]);
				break;
			default:
				usagexit(argv[0]);
			}
		} else { /* ( argv[arg][1] != '-' ) */
			switch ( unswitched++ ) {
			case 0:
				A.filename = argv[arg];
				break;
			default:
				usagexit(argv[0]);
			}
		}
	}
}

/*======================================================
 * initargs - set A structure to initial values
 *======================================================
*/
initargs()
{
	A.filename  = DEFFILE;
	A.baudrate = B9600;
	A.class = NULL;
	A.downloader = NULL;
	A.machnum = GD_ANYDEV;
	A.devflags = FALSE;
	A.reloadether = FALSE;
	A.verbose = FALSE;
	A.silent = FALSE;
	A.autostart = FALSE;
	A.startdelay = 0;
}

/*===========================================================
 * getdbargs - add argument string from database db to argvec
 *===========================================================
*/
getdbargs(Dev, argvec)
DevDesc *Dev;
char	*argvec[];
{
	char	*arglist;
	int	nargs;

	if (Dev != NULL) {
		arglist = get_args(Dev->class);
		arglist = skip_white(arglist);
		for (nargs = 1; *arglist != NULL && *arglist != QUOTE; ) {
			argvec[nargs++] = arglist;
			while( *arglist != NULL && *arglist != QUOTE &&
			      index(FIELD_END, *arglist) == 0 )
				arglist++;
			if (*arglist != NULL)
				*arglist++ = NULL;
			arglist = skip_white(arglist);
		}
	}
	return(--nargs);
}

/*
 *======================================================================
 * using_msg - print a message saying which dev is being downloaded to
 *======================================================================
*/
using_msg(Dev, sbProg)
DevDesc	*Dev;
char	*sbProg;
{
	if (Dev == NULL) {
		if (A.verbose)
			fprintf(stderr, "No devices Available.\n");
		exit(1);
	} else if (!A.silent && !A.verbose)
		printf("Using %s\n", shortname(Dev->machine));
	else if (A.verbose)
		printf("%s: Downloading to %s\n", sbProg, Dev->machine);
}

/*
 *=======================================
 * linemode - set up linemode and speed
 *=======================================
 */
linemode(fd,speed,mode,Dev)
int fd, speed, mode;
DevDesc	*Dev;
{
	struct sgttyb tcb;
	if ( gtty(fd,&tcb) < 0 ) {
		perror(get_name(Dev->machine));
		exit(1);
	}
	if (speed != NOBAUD)
		tcb.sg_ispeed = tcb.sg_ospeed = speed;
	tcb.sg_flags = mode;
	stty(fd,&tcb);
}

/*
 *==========================================
 * usagexit - print usage message and exit
 *==========================================
 */
usagexit(pgm)
char *pgm;
{
	fprintf(stderr,
	"usage: %s [filename] [-cCLASS] [-lMACHINE#] [-a[delay]] [-Bbaudrate] [-ddownloader] [-v] [-h] [-s] [-e]\n",
	pgm);
	exit(1);
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

	if ((fd=open(name,mode,0)) < 0 ) {
		perror(name);
		exit(1);
	}
	return(fd);
}
