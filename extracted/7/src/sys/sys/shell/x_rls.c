/* x_rls.c - x_rls */

#include <conf.h>
#include <kernel.h>
#include <shell.h>

/*------------------------------------------------------------------------
 *  x_rls  -  (command rls) list contents of remote file system directory
 *------------------------------------------------------------------------
 */
COMMAND	x_rls(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	char	*p, *buf;
	int	dev, len;
	char	str[32];
	struct	dirent	{		/* UNIX directory entry		*/
		long	d_inum;		/* file's inode number		*/
		short	d_rlen;		/* length of this record	*/
		short	d_nlen;		/* length of this file's name	*/
		char	d_nam[1];	/* start of file name		*/
	};
	struct	dirent	*d;
	Bool	aflag;


	aflag = FALSE;
	if (nargs > 1 && strcmp(p=args[1],"-a") == 0) {
		nargs--;
		aflag = TRUE;
		p = args[2];
	}
	if (nargs == 1)
		p = ".";
	else if (nargs != 2) {
		printf("use: rls [-a] directory\n");
		return(SYSERR);
	}
	if ( ((int)(buf=(char *)getmem(512))) == SYSERR) {
		fprintf(stderr, "rls: no memory\n");
		return(SYSERR);
	}
	if (nammap(p, buf) != RFILSYS ||
	   (dev=open(NAMESPACE, p, "ro")) == SYSERR) {
		fprintf(stderr, "cannot open %s\n", p);
		freemem(buf, 512);
		return(SYSERR);
	}
	len = read(dev, buf, 512);
	for( ; len > 0 ; len=read(dev, buf, 512)) {
		for (p=buf ; p< &buf[512] ;) {
			d = (struct dirent *)p;
			if (d->d_nlen != strlen(d->d_nam) ||
			    d->d_nlen > 32 || d->d_rlen < 0 ||
			    d->d_rlen > 512) {
				fprintf(stderr, "Not a directory\n");
				close(dev);
				freemem(buf, 512);
				return(SYSERR);
			}
			if (aflag || d->d_nam[0] != '.') {
				strcpy(str, d->d_nam);
				strcat(str, "\n");
				write(stdout, str, strlen(str));
			}
			if (d->d_rlen == 0)
				break;
			p += d->d_rlen;
		}
	}
	freemem(buf, 512);
	close(dev);
	return(OK);
}
