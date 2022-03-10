/* dlpack.h */

/* Declarations and Constants for downloader protocol */

/* Protocol: eload accepts command packets over the ethernet and	*/
/* issues the appropriate response via ethernet.  All packets are	*/
/* transmitted via UDP.  A minimal packet consists of a struct		*/
/* dl_header, with appropriate dl_op field, as defined below.  All	*/
/* ACKs & NAKs are minimal packets, with the exception of DLEAK, which	*/
/* is a full-sized dl_pack.  Packets with dl_op == DLDEP are also	*/
/* full-sized dl_packs.  DLSRT packets make use of a dl_header,		*/
/* followed by the dl_srtval structure, which defines start parameters	*/
/* for the downloaded image.  DLEXT, DLID, DLMIQ are minimal		*/
/* packets.								*/

#define	DLMAXD	1024			/* max data in packet		*/

/* values for dl_op command field in eload packet */
#define	DLACK		'\001'		/* download acknowledgment	*/
#define	DLEAK		'\002'		/* upload acknowledgment	*/
#define	DLNAK		'\003'		/* negative acknowledgment	*/
#define	DLRAK		'\004'		/* ready to run ack.		*/
#define	DLQAK		'\005'		/* am quitting ack.		*/
#define	DLENK		'\006'		/* upload out of bounds NAK	*/
#define	DLDEP		'\007'		/* deposit image to xinu machine*/
#define	DLEXT		'\010'		/* extract image from "  "	*/
#define	DLSRT		'\011'		/* start downloaded prog w parms*/
#define	DLID		'\012'		/* init. address info to eload	*/

struct	dl_header {			/* header for download packet	*/
	char	dl_op;			/* operation, as above		*/
	char	dl_vers;		/* current version no. of eload	*/
	short	dl_len;			/* length of data area		*/
	long	dl_addr;		/* source (dest.) memory address*/
};


struct	dl_srtval  {
	long	dl_entry;		/* downloaded image entry point	*/
	long	dl_delay;		/* delay time before running	*/
};

struct	dl_pack	{			/* format of download packet	*/
	struct	dl_header dl_hdr;	/* packet header		*/
	union	{			/* either data area or register	*/
		char	dl_data[DLMAXD];/* data to store (read) to mem.	*/
		struct dl_srtval dl_srt;/* start downloaded image parms.*/
	} dl_pt;
};
