/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include "../h/conf.h"

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  CONSOLE */
0,
ttyinit, ioerr, ioerr,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
030000000, 0035, 0035,
sluint, sluint, NULLPTR, 0,

/*  OTHER */
1,
ttyinit, ioerr, ioerr,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
030000000, 0035, 0035,
sluint, sluint, NULLPTR, 1,

/*  DISK0 */
2,
dsinit, dsopen, ioerr,
dsread, dswrite, dsseek,
ioerr, ioerr, dscntl,
07600000, 0034, 0034,
dsinter, dsinter, NULLPTR, 0,

/*  FILE0 */
3,
lfinit, ioerr, lfclose,
lfread, lfwrite, lfseek,
lfgetc, lfputc, ioerr,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  FILE1 */
4,
lfinit, ioerr, lfclose,
lfread, lfwrite, lfseek,
lfgetc, lfputc, ioerr,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 1,

/*  FILE2 */
5,
lfinit, ioerr, lfclose,
lfread, lfwrite, lfseek,
lfgetc, lfputc, ioerr,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 2,

/*  FILE3 */
6,
lfinit, ioerr, lfclose,
lfread, lfwrite, lfseek,
lfgetc, lfputc, ioerr,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 3
	};
