/*
 * odtunlock  --  unlock "my" machine or a specified machine
 */
#include <stdio.h>
#include "getdev.h"

main(argc, argv)
int argc;
char *argv[];
{
	char	class[MAXNAMELEN];
	int	machnum = MYDEV;

	strcpy(class, "LSI");
	for ( ; argc > 1; --argc) {
		if (**++argv != '-') {
			fprintf(stderr, "Argument %s unexpected.\n", *argv);
			exit(1);
		}
		switch (argv[0][1]) {
		case 'c':
			strcpy(class, argv[0] + 2);
			break;
		case 'l':
			if (sscanf(argv[0], "-l%d", &machnum) != 1) {
				fprintf(stderr, "%s: Illegal argument\n",
					argv[0]);
				exit(1);
			}
			break;
		default:
			fprintf(stderr, "Unknown flag: %s\n", argv[0]);
			exit(1);
		}
	}

	if ((machnum=freedev(class, machnum)) < 0)
		printf("Could not release %s #%d.  Check filemodes.\n",
			class, -machnum);
	else
		printf("%s #%d released.\n", class, machnum);
}
