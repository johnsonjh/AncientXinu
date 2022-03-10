/* getdev.h  -  constants for the device auto-select code */

#define TIMEOUT		10		/* minutes keyboard is allowed	*/
					/*  to be idle			*/

#define TTYNAME		-3
#define	MYDEV		-2
#define	ANYDEV		-1
#define MAXNAMELEN	30		/* maximum length of a file name*/

#define TMPDIR		"/tmp"
#define DEVDIR		"/dev"
#define TMPUSER		"%s/odtlock.%s"	/* template for tmp user file	*/
#define DEVNAME		"%s/%s.%%d"	/* template for device template	*/
					/*  (also used for lock file)	*/
