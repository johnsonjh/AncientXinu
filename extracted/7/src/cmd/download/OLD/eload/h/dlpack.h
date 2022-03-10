/* dlpack.h */

/* Declarations and Constants for downloader protocol */

/* Protocol: downloader delivers packets over the Ethernet and		*/
/* expects ACKs back over the serial line.  Normal packet contains	*/
/* a real memory address, 0 <= address <= 48K, length, and data to	*/
/* be placed at that address.  Special command packet contains the	*/
/* address value DLCMD, and the data area contains register values	*/
/* delay, and autostart code.  All packets are ACKed.			*/

#define	DLMAXD	1024			/* max data in packet		*/
#define	DLCMD	-1			/* commands have this address	*/
#define	DLPS	8			/* index of PS in reg table	*/
#define	DLDELAY	9			/* index of delay in reg table	*/
#define	DLSTART	10			/* index of start addr in table	*/

struct	dlpack	{			/* format of download packet	*/
	short	dl_addr;		/* destination memory address	*/
	short	dl_len;			/* length of data area only	*/
	union	{			/* either data area or register	*/
		char	dl_data[DLMAXD];/* data to store in memory	*/
		short	dl_regs[11];	/* r0-r7, PS, delay, start addr	*/
	}	dl;
	short	dl_cksum;		/* checksum (now unused)	*/
};
