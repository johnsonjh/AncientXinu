/*
 * LSIunlock  --  unlock "my" LSI 11 or a specified LSI 11
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "/usr/Xinu/include/getlsi.h"

char	*lsidevs[] = {
#include "/usr/Xinu/include/lsidevs.h"
		"/dev/LSI"};
#define	NDEVS	sizeof(lsidevs)/sizeof(char *)

char	*loginid;
char	tmplock[30];
char	tmpuser[30];
char	convert[30];

main(argc, argv)
int argc;
char *argv[];
{
	int	lsinum, len;
	int	fd;
	char	*getlogin();

        if ( argc <= 1 ) {
		strcpy(tmpuser,TMPNAME);
		if ( (loginid=getlogin()) == NULL)
			loginid = (getpwuid(getuid()))->pw_name;
		strcat(tmpuser,loginid);
		if ( (fd=open(tmpuser,0)) >= 0) {
			if ((len=read(fd,&lsinum,sizeof(int)))==sizeof(int)) {
				if (lsinum>0 && lsinum<NDEVS) {
					release(lsinum);
					unlink(tmpuser);
					exit(0);
				}
			}
		}
		fprintf(stderr, "No LSI on reserve.\n");
		exit(1);
	} else if ( argc > 2 ) {
		fprintf(stderr, "%s: too many arguments\n", argv[0]);
		exit(1);
	} else {
		if (strncmp(argv[1],"-l",2) == 0)
			argv[1] += 2;
		if ( (strlen(argv[1])<=0) || (lsinum=atoi(argv[1]))<0
			|| lsinum>=NDEVS) {
			fprintf(stderr, "%s: Illegal argument\n", argv[0]);
			exit(1);
		}
	}
	release(lsinum);
}

release(lsinum)
int	lsinum;
{
	strcpy(tmplock,TMPNAME);
	sprintf(convert, "%d", lsinum);
	strcat(tmplock,convert);
	if( (unlink(tmplock)) < 0 )
		fprintf(stderr, "Could not release LSI.  Check filemodes.\n");
	else
		fprintf(stderr, "LSI #%d released.\n", lsinum);
}
