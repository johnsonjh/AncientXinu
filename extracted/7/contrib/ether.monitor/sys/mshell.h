/* mshell.h */


/* Declarations for the pseudo shell */
#include <mark.h>

#define	MDONE		-5		/* mshell exit return		*/
#define	MNAMLEN		9		/* length of user name + 1	*/
#define	MBUFLEN		128		/* length of general buffer	*/
#define	MARGLEN		130		/* length of area for arguments	*/
#define	MMAXTOK		18		/* maximum tokens per line	*/

struct	mshvars	{			/* Mshell variables		*/
	long	mlast;			/* time shell last touched	*/
	long	mlogon;			/* time user logged on		*/
	int	mncmds;			/* number of commands available	*/
	MARKER	mmark;			/* marked if puse valid         */
	Bool	mused;			/* TRUE => user using shell	*/
	char	muser[MNAMLEN];		/* current user name		*/
	char	mmach[32];		/* name of this machine		*/
	char	mshbuf[MBUFLEN];	/* general purpose buffer	*/
	char	*mtokens[MMAXTOK];	/* pointers to input tokens	*/
	char	mtoktyp[MMAXTOK];	/* type of token in mtokens[i]	*/
	char	margstr[MARGLEN];	/* actual strings of arguments	*/
};

extern	struct	mshvars	Msh;
