/* cmd.h */

/* Declarations for all commands known by the shell */

struct	cmdent	{			/* entry in command table	*/
	char	*cmdnam;		/* name of command		*/
	Bool	cbuiltin;		/* Is this a builtin command?	*/
	int	(*cproc)();		/* procedure that implements cmd*/
};

extern	int
	x_bpool(),	x_date(),	x_devs(),	x_dg(),
	x_exit(),	x_help(),	x_kill(),	x_mem(),
	x_net(),	x_ps(),		x_reboot(),	x_routes();

/* Commands:	 name		Builtin?	procedure	*/

#define	CMDS 	"bpool",	FALSE,		x_bpool,	\
		"date",		FALSE,		x_date,		\
		"devs",		FALSE,		x_devs,		\
		"dg",		FALSE,		x_dg,		\
		"exit",		TRUE,		x_exit,		\
		"help",		FALSE,		x_help,		\
		"kill",		TRUE,		x_kill,		\
		"logout",	TRUE,		x_exit,		\
		"mem",		FALSE,		x_mem,		\
		"netstat",	FALSE,		x_net,		\
		"ps",		FALSE,		x_ps,		\
		"reboot",	TRUE,		x_reboot,	\
		"routes",	FALSE,		x_routes,	\
		"time",		FALSE,		x_date,		\
		"?",		FALSE,		x_help

extern	struct	cmdent	cmds[];
