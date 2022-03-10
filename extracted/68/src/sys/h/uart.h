/* uart.h  */

#define	NUFULL	1
#define NUEMPTY	4

struct	uart {
	char	u_data;
	char	u_pad1;
	char	u_status;
	char	u_pad2;
};
