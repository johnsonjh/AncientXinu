#
	char	*sccsid = "@(#)strip.c	2.4";
#include "/usr/Xinu/include/a.out.h"
#include <signal.h>

#ifndef UCB_NOPT
#define	UCB_NOPT		/* Leave namelist only on -n flag */
#endif

char	*tname;
char	*mktemp();
struct exec head, nhead;
int 	a_magic[] = {A_MAGIC1, A_MAGIC2, A_MAGIC3, A_MAGIC4,
#ifdef MENLO_OVLY
			0430, 0431,
#endif
			0};
int	status;
int	tf;
#ifdef	UCB_NOPT
int	nflg;
#endif

main(argc, argv)
char *argv[];
{
	register i;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	tname = mktemp("/tmp/sXXXXX");
	close(creat(tname, 0600));
	tf = open(tname, 2);
	if(tf < 0) {
		printf("cannot create temp file\n");
		exit(2);
	}
#ifdef	UCB_NOPT
	if((argv[1][0] == '-') && (argv[1][1] == 'n') && (argv[1][2] == '\0'))
	{
		nflg++;
		argc--;
		argv++;
	}
#endif
	for(i=1; i<argc; i++) {
		strip(argv[i]);
		if(status > 1)
			break;
	}
	close(tf);
	unlink(tname);
	exit(status);
}

strip(name)
char *name;
{
	register f;
	long size;
	int i;

	f = open(name, 0);
	if(f < 0) {
		printf("cannot open %s\n", name);
		status = 1;
		goto out;
	}
	read(f, (char *)&head, sizeof(head));
#ifdef MENLO_OVLY
	nhead = head;
#endif
	for(i=0;a_magic[i];i++)
		if(a_magic[i] == head.a_magic) break;
	if(a_magic[i] == 0) {
		printf("%s not in a.out format\n", name);
		status = 1;
		goto out;
	}
	if(head.a_syms == 0 && (head.a_flag&1) != 0) {
		printf("%s already stripped\n", name);
		goto out;
	}
	size = (long)head.a_text + head.a_data;
#ifdef	UCB_NOPT
	if(nflg)
	{
		lseek(f, (long)(size + sizeof(head)), 0);
		size = (long)head.a_syms;
		head.a_text = 0;
		head.a_data = 0;
		head.a_bss = 0;
	}
	else
	{
		head.a_syms = 0;
		head.a_flag |= 1;
	}
#else
	head.a_syms = 0;
	head.a_flag |= 1;

#endif
	lseek(tf, (long)0, 0);
	write(tf, (char *)&head, sizeof(head));
#ifdef MENLO_OVLY
	if (head.a_magic == 0431 || head.a_magic == 0430) {
		unsigned sizes[8];
		int i;

		read(f, (char *)&sizes, sizeof sizes);
		write(tf, (char *)&sizes, sizeof sizes);
		for (i = 1; i < 8; i++)
			copy(name, f, tf, (long) sizes[i]);
	}
#endif
	if(copy(name, f, tf, size)) {
		status = 1;
		goto out;
	}
#ifdef MENLO_OVLY
	lseek(f, (long)nhead.a_syms, 1);
	if (nhead.a_flag == 0)
		lseek(f, (long)head.a_text + (long)head.a_data, 1);
	qcopy(name, f, tf);
#endif
	size += sizeof(head);
	close(f);
	f = creat(name, 0666);
	if(f < 0) {
		printf("%s cannot recreate\n", name);
		status = 1;
		goto out;
	}
	lseek(tf, (long)0, 0);
	if(copy(name, tf, f, size))
		status = 2;
#ifdef MENLO_OVLY
	qcopy(name, tf, f);
#endif

out:
	close(f);
}

copy(name, fr, to, size)
char *name;
long size;
{
	register s, n;
	char buf[512];

	while(size != 0) {
		s = 512;
		if(size < 512)
			s = size;
		n = read(fr, buf, s);
		if(n != s) {
			printf("%s unexpected eof\n", name);
			return(1);
		}
		n = write(to, buf, s);
		if(n != s) {
			printf("%s unexpected write eof\n", name);
			return(1);
		}
		size -= s;
	}
	return(0);
}

#ifdef MENLO_OVLY
qcopy(name, fr, to)
char *name;
{
	register s, n;
	char buf[512];

	for(;;) {
		n = read(fr, buf, 512);
		if(n == 0)
			return;
		if(n < 0) {
			perror("read");
			return;
		}
		s = write(to, buf, n);
		if(n != s) {
			printf("%s unexpected write eof\n", name);
			return(1);
		}
	}
}
#endif
