/*
 * flags for the descriptor block -- the first three are the same as the bit
 * to be flipped in the	appropriate byte, the rest distinct from these three
 */
#define	VFY		0x1		/* read, write in verification mode */
#define	MTM		0x10		/* use maintenance mode	*/
#define	SAM		0x80		/* use 128 byte	addressing */
#define	OPEN		0x1		/* TU58	is open	*/
#define	READ		0x2		/* TU58	is to be read */
#define	WRITE		0x4		/* TU58	is to be written */
#define	ROPEN		(OPEN |	READ)	/* TU58	is open	for reading */
#define	WOPEN		(OPEN |	WRITE)	/* TU58	is open	for writing */
#define	RWOPEN		(OPEN |	READ | WRITE)	/* TU58	is open	for reading and	writing	*/

/*
 * some	internal constants
 */
#define	TU58FILE	"/dev/LSIfast"	/* device name for the TU58 */
#define	TU58LOCK	"/tmp/LSIlock"	/* lock	file name for the TU58 */
#define	IOBLKSZ		128		/* bytes per i/o block */
#define	SAMSIZE		128		/* bytes / block in SAM	*/
#define	NBLKSAM		(512 * 4)	/* blocks/tape in SAM */
#define	NORMSIZE	512		/* bytes / block not in	SAM */
#define	NBLKNORM	512		/* blocks/tape not in SAM */
#define	NDRIVES		2		/* number of drives on the TU58	*/
#define	MAXCHAR		1000		/* max INITS before giving up */
#define	SYSERR		-1		/* returned in some cases on error */

/*
 * one-byte commands
 */
#define	BDATA		0x1		/* byte	to signal data packet */
#define	BCTRL		0x2		/* byte	to signal command packet */
#define	BEND		0x2		/* byte	to signal end packet */
#define	BINIT		0x4		/* byte	to initialize TU58 */
#define	BCONT		0x10		/* byte	to resume sending bytes	*/
#define	BXOFF		0x13		/* byte	to stop	incoming bytes */
#define	CREAD		0x2		/* command to read from	TU58 */
#define	CWRITE		0x3		/* command to write to TU58 */
#define	CPOSIT		0x5		/* command to position the TU58	*/
#define	CEND		0x40		/* command signalling end packet */
/*
 * this	file includes the TU58 error messages
 */
#include <tu58errno.h>

/*
 * this	file includes the TU58 structure
 */
#include <tu58io.h>
