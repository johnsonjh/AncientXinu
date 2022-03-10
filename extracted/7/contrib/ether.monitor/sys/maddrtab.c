/* maddrtab.c - maddrtab */

#include <conf.h>
#include <kernel.h>
#include "../h/monitor.h
extern	Bool	found;

/*----------------------------------------------------------------------------
 * maddrtab - print the table that contains the information about physical
 *            network address, internet address, and host name 
 *----------------------------------------------------------------------------
 */

maddrtab(stdout)
int	stdout; 
{

	char	str[80];
	struct	name	*list;

	list = table;
	fprintf(stdout,"		Address Table:\n");
	fprintf(stdout,"physical network address	internet address	host name\n");
	while(!blkequ(list->n_nam, ZERO, strlen(list->n_nam))){
		sprintf(str,"%-20s		",list->n_epaddr);
		sprintf(&str[strlen(str)],"%-20s	",list->n_ipaddr);
		sprintf(&str[strlen(str)],"%-20s\n",list->n_nam);
		write(stdout, str, strlen(str));
		list++;
	}
	found = TRUE;
	return(OK);
}
