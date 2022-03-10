/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  Format of entries is:
device-number, device-name,
init, open, close,
read, write, seek,
getc, putc, cntl,
device-csr-address, input-vector, output-vector,
iint-handler, oint-handler, control-block, minor-device,
*/

/*  CONSOLE  is tty  */

0, "CONSOLE",
ttyinit, ttyopen, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
0177560, 0060, 0064,
ttyiin, ttyoin, NULLPTR, 0,

/*  ETHER  is eth  */

1, "ETHER",
ethinit, ioerr, ioerr,
ethread, ethwrite, ioerr,
ioerr, ioerr, ioerr,
0174440, 0400, 0400,
ethinter, ethinter, NULLPTR, 0,

/*  INTERNET  is dgm  */

2, "INTERNET",
ionull, dgmopen, ioerr,
ioerr, ioerr, ioerr,
ioerr, ioerr, dgmcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  DGRAM1  is dg  */

3, "DGRAM1",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0,

/*  DGRAM2  is dg  */

4, "DGRAM2",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 1,

/*  DGRAM3  is dg  */

5, "DGRAM3",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 2,

/*  DGRAM4  is dg  */

6, "DGRAM4",
dginit, ioerr, dgclose,
dgread, dgwrite, ioerr,
ioerr, ioerr, dgcntl,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 3,

/*  NAMESPACE  is nam  */

7, "NAMESPACE",
naminit, namopen, ioerr,
ioerr, ioerr, ioerr,
ioerr, ioerr, ioerr,
0000000, 0000, 0000,
ioerr, ioerr, NULLPTR, 0
	};
