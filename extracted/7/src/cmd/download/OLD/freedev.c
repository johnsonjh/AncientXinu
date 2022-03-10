/* freedev.c - freedev, release */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "getdev.h"

/*------------------------------------------------------------------------
 *  freedev  --  unlock machine #machnum of type class, or MYDEV
 *------------------------------------------------------------------------
 */
freedev(class, machnum)
char	*class;
int	machnum;
{
	char	*getlogin(), *rindex();
	char	*loginid;
	int	fd;
	int	len;
	char	tmpuser[MAXNAMELEN];

	if ( machnum != MYDEV )
		return(release(class, machnum));

	/* Find out what my device is.	*/
	if ( (loginid=getlogin()) == NULL || *loginid == '\0')
		loginid = getpwuid(getuid())->pw_name;
	sprintf(tmpuser, TMPUSER, TMPDIR, loginid);
	if ( (fd=open(tmpuser,0)) < 0) {
		fprintf(stderr, "No %s on reserve.\n", class);
		exit(1);
	}
	len = read(fd, class, MAXNAMELEN);
	close(fd);
	class[len - 1] = '\0';
	if (len < 3 ||
		sscanf(rindex(class, '.'), ".%d", &machnum) != 1) {
		fprintf(stderr, "%s is unintelligible.\n", tmpuser);
		exit(1);
	}
	if (machnum < 1) {
		fprintf(stderr, "%d is not a valid device number.\n",
			machnum);
		exit(1);
	}
	*rindex(class, '.') = '\0';
	if ((machnum=release(class, machnum)) > 0)
		unlink(tmpuser);
	return(machnum);
}

release(class, machnum)
char	*class;
int	machnum;
{
	char	tmpname[MAXNAMELEN];
	char	tmplock[MAXNAMELEN];

	sprintf(tmpname, DEVNAME, TMPDIR, class);
	if (unlink(sprintf(tmplock, tmpname, machnum)) < 0)
		return(-machnum);
	else
		return(machnum);
}
