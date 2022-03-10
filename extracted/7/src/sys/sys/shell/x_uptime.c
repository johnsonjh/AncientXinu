/* x_uptime.c - x_uptime */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <rwho.h>

/*------------------------------------------------------------------------
 *  x_uptime  -  (command uptime or ruptime) print remote machine status
 *------------------------------------------------------------------------
 */
COMMAND	x_uptime(stdin, stdout, stderr, nargs, args)
int	stdin, stdout, stderr, nargs;
char	*args[];
{
	int	i, j;
	struct	rwent	*rwptr;
	long	tottim;
	int	hours, days, mins;
	Bool	up;
	Bool	found, all;
	long	now;
	char	mach[32];
	char	str[80];

	switch (nargs) {

	case 1:
		if (strcmp(args[0],"ruptime") == 0) {
			all = TRUE;
			break;
		}
		getname(mach); /* called as "uptime" */
		args[1] = mach;
		/* fall through */
	case 2:
		all = FALSE;
		break;
	default:
		fprintf(stderr, "use: %s [host]\n", args[0]);
		return(SYSERR);
	}
	gettime(&now);
	found = FALSE;
	for (i=0 ; i<Rwho.rwnent ; i++) {
		rwptr = &Rwho.rwcache[i];
		if (!all && strcmp(rwptr->rwmach,args[1])!=0)
			continue;
		found = TRUE;
		sprintf(str,"%-14s", rwptr->rwmach);
		tottim = now - rwptr->rwlast;
		if (up = (tottim < RWCDOWN))
			tottim = rwptr->rwslast - rwptr->rwboot + 59;
		days = tottim / (24L * 60L * 60L);
		 tottim %= (24L * 60L * 60L);
		hours = tottim / (60L * 60L);
		 tottim %= (60L * 60L);
		mins = tottim / 60L;
		sprintf(&str[strlen(str)], "%s", up?"up  " : "down");
		if (days > 0)
			sprintf(&str[strlen(str)], " %2d +", days);
		else
			strcat(str, "     ");
		sprintf(&str[strlen(str)], " %2d:%02d", hours, mins);
		if (!up) {
			strcat(str, "\n");
			write(stdout, str, strlen(str));
			continue;
		}
		sprintf(&str[strlen(str)],
			"  %2d users,    load ", (int)rwptr->rwusers);
		for (j=0 ; j<RWNLOAD ; j++) {
			if (j >0)
				strcat(str, ", ");
			sprintf(&str[strlen(str)], "%2d.%02d",
				rwptr->rwload[j] / 100,
				rwptr->rwload[j] % 100);
		}
		strcat(str, "\n");
		write(stdout, str, strlen(str));
	}
	if (!found && !all)
		fprintf(stderr, "%s ???\n", args[1]);
	return(OK);
}
