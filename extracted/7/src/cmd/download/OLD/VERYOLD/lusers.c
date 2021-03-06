/*
 * lusers  --  list users who have an LSI 11 on reserve
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "/usr/Xinu/include/getlsi.h"

char	*lsidevs[] = {
#include "/usr/Xinu/include/lsidevs.h"
		"/dev/LSI"};
#define	NDEVS	sizeof(lsidevs)/sizeof(char *)
char	tmplock[30];
char	convert[30];

main(argc, argv)
int argc;
char *argv[];
{
	int	i;
	char	user[200];
	char	*index();
	int	idle;
	struct	stat	sbuf;
	long	now, time();

        if ( argc > 1 ) {
		fprintf(stderr, "%s: argument unexpected\n", argv[0]);
		exit(1);
	}
	for (i=0 ; i<NDEVS ; i++) {
		if (access(lsidevs[i],4) < 0)
			continue;
		strcpy(tmplock, TMPNAME);
		sprintf(convert, "%d", i);
		strcat(tmplock, convert);
		if (stat(tmplock, &sbuf) < 0)
			continue;
		getpw(sbuf.st_uid, user);
		*index(user, ':') = '\0';
		now = time(0L);
		idle = (( (int) (now-sbuf.st_mtime) ) + 30) / 60;
		printf("LSI #%d  idle time = %3d mins.  Reserved by %4s\n",
			i, idle, user);
	}
}
