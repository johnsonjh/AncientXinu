#define HOST VAX
static	char sccsid[] = "@(#)cc.c 4.1 4/22/83";
/*
 * cc - front end for C compiler
 */
#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <dir.h>

char	*cpp = "/lib/cpp";
char	*ccom68 = "/v1/burns/Xinu68/bin/ccom68";
char	*c268 = "/v1/burns/Xinu68/bin/c268";
char    *as68 = "/v1/burns/Xinu68/bin/as68";
char    *ld68 = "/v1/burns/Xinu68/bin/ld68";
char	*crt068 = "/v1/burns/Xinu68/lib/startup.o";
char	*mcrt068 = "/v1/burns/Xinu68/lib/startup.o";

char	tmp0[30];		/* big enough for /tmp/ctm%05.5d */
char	*tmp1, *tmp2, *tmp3, *tmp4, *tmp5;
char	*outfile;
char	*savestr(), *strspl(), *setsuf();
int	idexit();
char	**av, **clist, **llist, **plist;
int	cflag, eflag, gflag, oflag, pflag;
int	sflag, wflag, Rflag, exflag, proflag;
char	*tflag;
char	*dflag;
int	exfail;
char	*chpass;
char	*npassname;

int	nc, nl, np, nxo, na;

#define	cunlink(s)	if (s) unlink(s)

main(argc, argv)
	char **argv;
{
	char *t;
	char *assource;
	int i, j, c;

	/* ld currently adds upto 5 args; 10 is room to spare */
	av = (char **)calloc(argc+10, sizeof (char **));
	clist = (char **)calloc(argc, sizeof (char **));
	llist = (char **)calloc(argc, sizeof (char **));
	plist = (char **)calloc(argc, sizeof (char **));
	for (i = 1; i < argc; i++) {
		if (*argv[i] == '-') switch (argv[i][1]) {

		case 'S':
			sflag++;
			cflag++;
			continue;
		case 'o':
			if (++i < argc) {
				outfile = argv[i];
				switch (getsuf(outfile)) {

				case 'c':
				case 'o':
					error("-o would overwrite %s",
					    outfile);
					exit(8);
				}
			}
			continue;
		case 'R':
			Rflag++;
			continue;
		case 'O':
			oflag++;
			continue;
		case 'p':
			proflag++;
			continue;
		case 'g':
			gflag++;
			continue;
		case 'w':
			wflag++;
			continue;
		case 'E':
			exflag++;
		case 'P':
			pflag++;
			if (argv[i][1]=='P')
				fprintf(stderr,
	"cc: warning: -P option obsolete; you should use -E instead\n");
			plist[np++] = argv[i];
		case 'c':
			cflag++;
			continue;
		case 'D':
		case 'I':
		case 'U':
		case 'C':
			plist[np++] = argv[i];
			continue;
		case 't':
			if (chpass)
				error("-t overwrites earlier option", 0);
			chpass = argv[i]+2;
			if (chpass[0]==0)
				chpass = "012p";
			continue;
		case 'T':	
			tflag = argv[++i];
			continue;
		case 'B':
			if (npassname)
				error("-B overwrites earlier option", 0);
			npassname = argv[i]+2;
			if (npassname[0]==0)
				npassname = "/usr/c/o";
			continue;
		case 'd':
			dflag = argv[i];
			continue;
		}
		t = argv[i];
		c = getsuf(t);
		if (c=='c' || c=='s' || exflag) {
			clist[nc++] = t;
			t = setsuf(t, 'o');
		}
		if (nodup(llist, t)) {
			llist[nl++] = t;
			if (getsuf(t)=='o')
				nxo++;
		}
	}
	if (gflag) {
		if (oflag)
			fprintf(stderr, "cc: warning: -g disables -O\n");
		oflag = 0;
	}
	if (npassname && chpass ==0)
		chpass = "012p";
	if (chpass && npassname==0)
		npassname = "/v1/burns/Xinu68/bin";
	if (chpass)
	for (t=chpass; *t; t++) {
		switch (*t) {

		case '0':
			ccom68 = strspl(npassname, "ccom");
			continue;
		case '2':
			c268 = strspl(npassname, "c2");
			continue;
		case 'p':
			cpp = strspl(npassname, "cpp");
			continue;
		}
	}
	if (proflag)
			crt068 = mcrt068;
	if (nc==0)
		goto nocom;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, idexit);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, idexit);
	if (pflag==0)
		sprintf(tmp0, "/tmp/ctm%05.5d", getpid());
	tmp1 = strspl(tmp0, "1");
	tmp2 = strspl(tmp0, "2");
	tmp3 = strspl(tmp0, "3");
	if (pflag==0)
		tmp4 = strspl(tmp0, "4");
	if (oflag)
		tmp5 = strspl(tmp0, "5");
	/* For each file on the command line */
	for (i=0; i<nc; i++) {
		if (nc > 1) {
			printf("%s:\n", clist[i]);
			fflush(stdout);
		}
		/* If we have a .s file, assemble it */
		if (getsuf(clist[i]) == 's') {
			assource = clist[i];
			goto assemble;
		} else
			assource = tmp3;
		if (pflag)
			tmp4 = setsuf(clist[i], 'i');
		/* Do the C-preprocessor pass */
		av[0] = "cpp"; av[1] = clist[i]; av[2] = exflag ? "-" : tmp4;
		na = 3;
		for (j = 0; j < np; j++)
			av[na++] = plist[j];
		av[na++] = 0;
		if (callsys(cpp, av)) {
			exfail++;
			eflag++;
		}
		if (pflag || exfail) {
			cflag++;
			continue;
		}
		/* Do the one pass compiler */
		if (sflag)
			assource = tmp3 = setsuf(clist[i], 's');
		av[0] = "ccom68";
		av[1] = tmp4; av[2] = oflag?tmp5:tmp3; na = 3;
		if (gflag)
			av[na++] = "-Xg";
		if (wflag)
			av[na++] = "-w";
		av[na] = 0;
		if (callsys(ccom68, av))
		{
			cflag++;
			eflag++;
			continue;
		}
		/* Do the optional pass 2. */
		if (oflag) {
			av[0] = "c268";
			av[1] = tmp5; av[2] = tmp3; av[3] = 0;
			if (callsys(c268, av)) {
				unlink(tmp3);
				tmp3 = assource = tmp5;
			} else
				unlink(tmp5);
		}
		if (sflag)
			continue;
	/* Assembly phase. */
	assemble:
		cunlink(tmp1); cunlink(tmp2); cunlink(tmp4);
		av[0] = "as68";
		av[1] = "-o"; av[2] = setsuf(clist[i], 'o');
		na = 3;
		/* Need to add this one to the 68000 assembler */
		if (Rflag)
			av[na++] = "-R";
		av[na++] = assource;
		av[na] = 0;
		if (callsys(as68, av) > 1) {
			cflag++;
			eflag++;
			continue;
		}
	}
nocom:
/* Linkage phase */
	if (cflag==0 && nl!=0) {
		i = 0;
		av[0] = "ld68"; 
		av[1] = "-N";
		na = 2;
		if (tflag) {
			av[na++] = "-T";
			av[na++] = tflag;
		}
		av[na++] = crt068;
		av[na++] = "-U";	
		if (outfile) {
			av[na++] = "-o";
			av[na++] = outfile;
		}
		while (i < nl)
			av[na++] = llist[i++];
		if (gflag)
			av[na++] = "-lg";
		av[na++] = 0;
		eflag |= callsys(ld68, av);
		if (nc==1 && nxo==1 && eflag==0)
			unlink(setsuf(clist[0], 'o'));
	}
	dexit();
}

idexit()
{

	eflag = 100;
	dexit();
}

dexit()
{

	if (!pflag) {
		cunlink(tmp1);
		cunlink(tmp2);
		if (sflag==0)
			cunlink(tmp3);
		cunlink(tmp4);
		cunlink(tmp5);
	}
	exit(eflag);
}

error(s, x)
	char *s, *x;
{
	FILE *diag = exflag ? stderr : stdout;

	fprintf(diag, "cc: ");
	fprintf(diag, s, x);
	putc('\n', diag);
	exfail++;
	cflag++;
	eflag++;
}

getsuf(as)
char as[];
{
	register int c;
	register char *s;
	register int t;

	s = as;
	c = 0;
	while (t = *s++)
		if (t=='/')
			c = 0;
		else
			c++;
	s -= 3;
	if (c <= DIRSIZ && c > 2 && *s++ == '.')
		return (*s);
	return (0);
}

char *
setsuf(as, ch)
	char *as;
{
	register char *s, *s1;

	s = s1 = savestr(as);
	while (*s)
		if (*s++ == '/')
			s1 = s;
	s[-1] = ch;
	return (s1);
}

callsys(f, v)
	char *f, **v;
{
	int t, status;

	t = vfork();
	if (t == -1) {
		printf("No more processes\n");
		return (100);
	}
	if (t == 0) {
		execv(f, v);
		printf("Can't find %s\n", f);
		fflush(stdout);
		_exit(100);
	}
	while (t != wait(&status))
		;
	if ((t=(status&0377)) != 0 && t!=14) {
		fprintf(stderr,"%s %s %s %s %s %s %s %s %s\n",
			f, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
		if (t!=2) {
			printf("Fatal error in %s\n", f);
			eflag = 8;
		}
		dexit();
	}
	return ((status>>8) & 0377);
}

nodup(l, os)
	char **l, *os;
{
	register char *t, *s;
	register int c;

	s = os;
	if (getsuf(s) != 'o')
		return (1);
	while (t = *l++) {
		while (c = *s++)
			if (c != *t++)
				break;
		if (*t==0 && c==0)
			return (0);
		s = os;
	}
	return (1);
}

#define	NSAVETAB	1024
char	*savetab;
int	saveleft;

char *
savestr(cp)
	register char *cp;
{
	register int len;

	len = strlen(cp) + 1;
	if (len > saveleft) {
		saveleft = NSAVETAB;
		if (len > saveleft)
			saveleft = len;
		savetab = (char *)malloc(saveleft);
		if (savetab == 0) {
			fprintf(stderr, "ran out of memory (savestr)\n");
			exit(1);
		}
	}
	strncpy(savetab, cp, len);
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return (cp);
}

char *
strspl(left, right)
	char *left, *right;
{
	char buf[BUFSIZ];

	strcpy(buf, left);
	strcat(buf, right);
	return (savestr(buf));
}
