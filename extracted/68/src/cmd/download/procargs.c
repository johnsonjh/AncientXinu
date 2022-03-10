/* procargs.c - procargs */

#include "define.h"

/*------------------------------------------------------------------------------
 * procargs  --  parse and check command line, setup global A structure
 *------------------------------------------------------------------------------
 */
procargs(argc, argv)
	int	argc;
	char	*argv[];
{
	int	arg;
	int	unswitched;

	A.fastloader	= FASTLOADER;
	A.bootloader	= BOOTLOADER;
	A.userprog	= USERPROG;
	A.logfile	= LOGFILE;
	A.loadaddr	= PROGSTART;
	A.verbose	= FALSE;
	A.silent	= FALSE;
	A.autostart	= FALSE;
	A.startdelay = 0;
	unswitched = 0;
	for ( arg=1 ; arg<argc ; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			switch ( argv[arg][1] ) {
				case 't':
					if ( ++arg >= argc )
						usagexit(argv[0]);
					break;
				case 'b':
					if ( ++arg >= argc )
						usagexit(argv[0]);
					A.bootloader = argv[arg];
					break;
				case 'f':
					if ( ++arg >= argc )
						usagexit(argv[0]);
					A.fastloader = argv[arg];
					break;
				case 'l':
					if ( ++arg >= argc )
						usagexit(argv[0]);
					A.loadaddr = atoi(argv[arg]);
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
					usagexit(argv[0]);
					break;
			}
		} else /* ( argv[arv][1] != '-' ) */ {
			switch ( ++unswitched ) {
			case 1:
				A.userprog = argv[arg];
				break;
			default:
				usagexit(argv[0]);
			}
		}
	}
}
