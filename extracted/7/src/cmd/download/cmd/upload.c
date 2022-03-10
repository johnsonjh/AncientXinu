/* upload.c -- upload */

#include "/usr/include/stdio.h"
#include "/usr/include/sgtty.h"
#include "/usr/include/ctype.h"
#include <baud.h>
#include <getdev.h>
#include "/usr/include/sys/file.h"
#include <upload.h>

static	struct	arglist A;
static	char	execfile[MAXBIN] = {LIBPATH};
struct	sgttyb	VAXttyb;

/*==============================================
 * upload -- outer layer of generic uploader
 *==============================================
*/
main(argc, argv)
int	argc;
char	*argv[];
{
	DevDesc	*Dev;
	int	i;
	char	*argvec[MAXARGS];

	procargs(argc, argv);	/* process arguments into struct A */
	Dev = getdev(A.class, A.machnum, A.devflags);
	if ((char *)Dev == NULL)
		exit(1);
	using_msg(Dev, argv[0]);
	linemode(Dev->fd, A.baudrate, RAW, Dev);
	gtty(Dev->fd, &VAXttyb);
	dup2(Dev->fd, DEVFD);	/* make the device the standard output  */
				/* copy arguments for execv call	*/
	for ( i = 1 ; i < argc && i < MAXARGS-3 ; i++)
		argvec[i] = argv[i];
	argvec[i++] = Dev->machine;
	argvec[i++] = Dev->locknm;
	if (i >= MAXARGS) {
		fprintf(stderr,"%s: too many arguments\n", argv[0]);
		exit(1);
	}
	argvec[i] = NULL;
	strncat(execfile, ((A.uploader == NULL) ? get_upload(Dev->machine)
			: A.uploader), MAXBIN - strlen(execfile) - 1);
	argvec[0] = execfile;
	if (A.verbose)
		fprintf(stderr,"exec binary %s\n",execfile);
	execv(execfile, argvec); /* run the uploader specified in db */
	perror(execfile);	/* execv failed */
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
	int arg, i, unswitched;
	/*
	 * assign default values
	 */
	A.uploader = NULL;
	A.lowaddr = 0;
	A.highaddr = 0;
	A.baudrate = B9600;
	A.class = NULL;
	A.machnum = GD_ANYDEV;
	A.devflags = FALSE;
	A.bootloader = NULL;
	A.verbose = FALSE;
	A.silent = FALSE;
	A.noload = FALSE;
	unswitched = 0;
	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' )
			switch ( argv[arg][1] ) {
			case 'u':
				if ( arg+1 >= argc )
					usagexit(argv[0]);
				A.uploader = argv[arg++];
				break;
			case 'o':
				if ( arg+1 >= argc )
					usagexit(argv[0]);
				break;
			case 'v':
				A.verbose = TRUE;
				break;
			case 's':
				A.silent = TRUE;
				break;
			case 'h':
				A.devflags |= GD_HLDDEVS;
				break;
			case 'n':
				A.noload = TRUE;
				break;
			case 'a':
				if (arg+1 >= argc)
					usagexit(argv[0]);
				/* leave the rest for the exec'd uploader */
				break;
			case 'c':
				A.class = argv[arg] + 2;
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
						argv[arg] + 2) == 0)
							break;
				if (i < NBAUD) {
					fprintf(stderr,
						"baud = %s (%d)\n",
						baudlist[i].name,
						baudlist[i].rate);
					A.baudrate = baudlist[i].rate;
				}
				else {
					fprintf(stderr,
					    "Unknown baud rate: %s\n",
						argv[arg]+1);
					exit(1);
				}
				break;
			default:
				usagexit(argv[0]);
			}
	else /* there's no dash in front */
		switch ( unswitched++ )	{
		case 0:
			/* let exec'd uploader do the rest */
			break;
		case 1:
			/* defer to exec'd uploader */
			break;
		default:
			usagexit(argv[0]);
		}
	}
}

/*
 *======================================================================
 * using_msg - print a message saying which dev is being uploaded to
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
		printf("%s: Uploading from %s\n", sbProg, Dev->machine);
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
	"usage: %s [filename] [-cCLASS] [-lLSI#] [-o corefile] [-Bbaudrate] [-u uploader] [-v] [-h] [-s]\n",
	pgm);
	exit(1);
}
