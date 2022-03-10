/*
 * odtunlock  --  unlock "my" machine or a specified machine
 */
#include "/usr/include/stdio.h"
#include <freedev.h>
#include <getdev.h>
#include <ulock.h>

main(argc, argv)
int argc;
char *argv[];
{
	char	*class, *user_rsrvd;
	int	len;
	int	machnum = FD_ANYDEV;

	class = NULL;
	for ( ; argc > 1; --argc) {
		if (**++argv != '-') {
			fprintf(stderr, "Argument %s unexpected.\n", *argv);
			exit(1);
		}
		switch (argv[0][1]) {
		case 'c':
			class = argv[0] + 2;
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

	get_db();
	user_rsrvd = get_ulock(SELF, &len);
	len = freedev(class, machnum, user_rsrvd, FD_FREE | FD_VERBOSE);
	put_ulock(SELF, user_rsrvd, len);
}
