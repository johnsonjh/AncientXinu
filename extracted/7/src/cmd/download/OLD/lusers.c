/*
 * lusers  --  list users who have an LSI 11 on reserve
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "getdev.h"

char	devname[MAXNAMELEN];
char	dev[MAXNAMELEN];
char	tmpuser[MAXNAMELEN];
char	tmplock[MAXNAMELEN];
char	lockname[MAXNAMELEN];

main(argc, argv)
int argc;
char *argv[];
{
	char	*getlogin();
	int	fd, len;
	char	class[MAXNAMELEN];
	int	lsinum = ANYDEV;
	int	i, nusers;
	char	*rindex();
	char	*user;
	int	idle;
	struct	stat	sbuf;
	long	now, time();

	strcpy(class, "");
	for ( ; argc > 1; --argc) {
		if (**++argv != '-') {
			fprintf(stderr, "Argument %s unexpected.\n", *argv);
			exit(1);
		}
		switch (argv[0][1]) {
		case 'c':
			strcpy(class, argv[0] + 2);
			break;
		default:
			fprintf(stderr, "Unknown flag: %s\n", argv[0]);
			exit(1);
		}
	}

	if (strcmp(class, "") == 0) {	/* Figure out the class.	*/
		if ( (user=getlogin()) == NULL || *user == '\0')
			user = getpwuid(getuid())->pw_name;
		sprintf(tmpuser, TMPUSER, TMPDIR, user);
		if ((fd = open(tmpuser, 2)) < 0)
			strcpy(class, "LSI");
		else {
			len = read(fd, class, sizeof(class));
			close(fd);
			class[len - 1] = '\0';
			*rindex(class, '.') = '\0';
		}
	}

	sprintf(tmplock, DEVNAME, TMPDIR, class);
	sprintf(devname, DEVNAME, DEVDIR, class);

	nusers = 0;
	for (i = 1; ; i++) {
		if (access(sprintf(dev, devname, i), 0) < 0) {
			if (i > 1)
				break;
			else {
				printf("No devices of class %s\n", class);
				exit(1);
			}
		}
		if (stat(sprintf(lockname, tmplock, i), &sbuf) < 0)
			continue;
		++nusers;
		user = getpwuid(sbuf.st_uid)->pw_name;
		now = time(0L);
		idle = (( (int) (now-sbuf.st_mtime) ) + 30) / 60;
		printf("%s #%d  idle time = %3d mins.  Reserved by %s\n",
			class, i, idle, user);
	}
	if (nusers == 0)
		printf("No one using %s devices.\n", class);
}
