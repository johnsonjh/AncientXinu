#include <signal.h>
#include <stdio.h>
#include <sgtty.h>

#define	BUFSZ 2048

char	buf[BUFSZ];
int	file;
char	outline[100];
int	nl = 0;
int	pos;
struct	sgttyb	oldtty, newtty;
int	timeout(), restore();

main(argc,argv)
	int	argc;
	char	**argv;
{
	int	i;
	long	timer;
	char	c;

	if(argc!=2)
		exit();
	if((file=open(argv[1],0))<=0)
		exit();
	gtty(0,&oldtty);		/*  Save old terminal state */
	gtty(0,&newtty);
	newtty.sg_flags &= ~(ALLDELAY|ECHO|CRMOD);
	newtty.sg_flags |= (CBREAK|CR2|NL2);   /* RAW was in too */
	stty(0,&newtty);
	write(1,"\\",1);
	while ( (read(0,&c,1) > 0) && (c != '>') ) ;

	for(;;) {
		if((i=getline())<=0) {
			break;
		}
		write(1,outline,i);
		signal(SIGALRM, timeout);
		alarm(10);
		while((read(0,&c,1)>0)&&(c!='>'));
		alarm(0);
	}

	write(1,"I A\r\n",5);
	restore();
	exit();
}


getline()
{
	int i;

	i= 0;
	for(;;) {	
		if(nl<=0)
		    if((nl=read(file,buf,BUFSZ))<=0)
			return(0);	
		    else
			pos= 0;
		outline[i]= buf[pos++];
		nl--;
		if(outline[i]=='\n')
		    break;
		i++;
		}
	return(i);
}

timeout()
{
	restore();
	exit();
}

restore()
{
	stty(0,&oldtty);
	close(file);
}
