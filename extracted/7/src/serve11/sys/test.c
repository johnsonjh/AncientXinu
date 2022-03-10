/* test.c - main */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <serve11.h>

/*------------------------------------------------------------------------
 *  main  --  Run all the servers Xinu needs for isolated sites
 *------------------------------------------------------------------------
 */
main()
{
	char	line[80];
	int	n;
	int	tserver(), nserver(), fserver();

	kprintf("\n");
	if (serinfo.ser_time) {
		kprintf("creating time server...\n");
		resume(create(tserver,400,30,"tserver",0));
	}
	if (serinfo.ser_name) {
		kprintf("creating name server...\n");
		resume(create(nserver,450,30,"nserver",0));
	}
	if (serinfo.ser_file) {
		kprintf("creating file server...\n");
		resume(create(fserver,400,30,"fserver",0));
	}
	if (serinfo.ser_rarp || serinfo.ser_time || serinfo.ser_name
		|| serinfo.ser_file)
		kprintf("Servers started.\n");
	else
		kprintf("Warning: no servers specified!  Why??");

	/* run shell */

	while (TRUE) {
		login(CONSOLE);
		shell(CONSOLE);
	}
}
