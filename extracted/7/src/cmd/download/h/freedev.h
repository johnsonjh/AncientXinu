#define FD_HOLD		(1 << 0)	/* hold dev--free others in class */
#define FD_FREE		(1 << 1)	/* free just this device	  */
#define FD_RELES	(1 << 2)	/* release other user's device	  */
#define FD_DBENTRY	(1 << 3)	/* class param is dev db entry	  */
#define FD_VERBOSE	(1 << 5)	/* verbose output when freeing	  */

#define FD_ANYDEV	(-1)

#define TRUE		1
#define FALSE		0
