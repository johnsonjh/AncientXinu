#include "define.h"

/*
 *==========================================
 * usagexit - print usage message and exit
 *==========================================
 */
usagexit(pgm)
char *pgm;
{
	fprintf(stderr,
	"usage: %s [filename] [-t tty] [-b filename] [-v] [-s] [-a [delay]]",
		pgm);
	callexit(1);
}

