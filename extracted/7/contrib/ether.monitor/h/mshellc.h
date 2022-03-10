/* mshellc.h */

/* This file declares the set of commands for the pseudo shell */

struct	mcmdent	{			/* entry in command table	*/
	char	*cmdnam;		/* name of command		*/
	int	(*cproc)();		/* procedure that implements cmd*/
};

extern	 mhelp(), mep(), mip(), marp(), mrarp(), mudp(), mtcp(), micmp();

#define	CMDS	"help",		mhelp,		\
		"udp",		mudp,		\
		"tcp",		mtcp,		\
		"icmp",		micmp,		\
		"ip",		mip,		\
		"arp",		marp,		\
		"rarp",		mrarp,		\
		"ep",		mep
extern	struct	mcmdent	mcmds[];



