/* deqna.h */

/* Definitions Digital Equipment Corporation's DEQNA Ethernet interface	*/

/* Device register layout */

struct	dqregs	{			/* deqna registers in I/O space	*/
	int	d_addr;			/* when read, low order byte of	*/
	int	d_addr2;		/*  these and next 4 regs give	*/
					/*  6-byte Ethernet node address*/
	char	*d_rcmd;		/* list of read cmds in memory	*/
	char	*d_rcmdh;		/* high order bits of d_rcmd	*/
	char	*d_wcmd;		/* list of write cmds in memory	*/
	char	*d_wcmdh;		/* high order bits of d_wcmd	*/
	char	*d_vect;		/* interrupt vector address	*/
	int	d_csr;			/* control and status register	*/
};

/* Bits in the Control and Status Register */

#define	DQ_ENBL		0000001		/* Receiver enable		*/
#define	DQ_REST		0000002		/* Software reset		*/
#define	DQ_NXMI		0000004		/* Nonexistent memory interrupt	*/
#define	DQ_BROM		0000010		/* Fetch boot/diagnostic ROM	*/
#define	DQ_XLI		0000020		/* Xmit list invalid		*/
#define	DQ_RLI		0000040		/* Receive list invalid		*/
#define	DQ_IEN		0000100		/* Interrupt Enable		*/
#define	DQ_XINT		0000200		/* Xmit interrupt		*/
#define	DQ_ILOP		0000400		/* Internal loopback mode	*/
#define	DQ_ELOP		0001000		/* External loopback mode	*/
#define	DQ_TIME		0002000		/* Sanity timer enable		*/
#define	DQ_RES1		0004000		/* Vendor reserved		*/
#define	DQ_FUSE		0010000		/* Fuse OK			*/
#define	DQ_CARR		0020000		/* Carrier present on ether	*/
#define	DQ_RES2		0040000		/* Vendor reserved		*/
#define	DQ_RINT		0100000		/* Receive interrupt		*/

/* Command to device as it appears on buffer descriptor list in memory	*/

struct	dcmd	{			/* DEQNA command in memory	*/
	int	dc_flag;		/* flag word marked USED/UNUSED	*/
	int	dc_bufh;		/* desc.+ high  buf. addr. bits	*/
	char	*dc_buf;		/* buffer addr. (low order bits)*/
	int	dc_len;			/* buffer length (see manual)	*/
	int	dc_st1;			/* Status word 1		*/
	int	dc_st2;			/* Status word 2		*/
};

/* Bits in the flag word of dcm */

#define	DC_NUSED	0100000		/* This dcmd entry not in use	*/
#define DC_USED		0140000		/* DEQNA using this dcmd entry	*/

/* Bits in the descriptor word of dcm (i.e. in dc_bufh) */

#define	DC_VALID	0100000		/* This is a valid buffer addr.	*/
#define	DC_CADDR	0040000		/* This is a chain address	*/
#define	DC_ENDM		0020000		/* Xmit: end of message		*/
#define	DC_SETUP	0010000		/* Xmit: setup packet		*/
#define	DC_NORM		0000000		/* Xmit: non-setup packet	*/
#define	DC_ENDL		0000000		/* Last descriptor in list	*/
#define	DC_XRES		0007400		/* Vendor reserved		*/
#define	DC_LBIT		0000200		/* Buffer ends on odd address	*/
#define	DC_HBIT		0000100		/* Buffer starts on odd address	*/

/* Bits returned to first status word (dc_st1) */

#define	DC_LUSE		0140000		/* LAST/USE bits (see manual)	*/
#define	DC_INIT		0100000		/* Initial value of status word	*/
#define	DC_ERRU		0040000		/* ERROR/USED bit (see manual)	*/
#define	DC_LOSS		0010000		/* Xmit: lost carrier		*/
#define	DC_NCAR		0004000		/* Xmit: no carrier		*/
#define	DC_ST16		0002000		/* Xmit: timer enabled		*/
#define	DC_ABRT		0001000		/* Xmit: abort			*/
#define	DC_FAIL		0000400		/* Xmit: heartbeat failure	*/
#define	DC_CCNT		0000360		/* Xmit: collision count	*/
#define	DC_ESET		0020000		/* Recv: setup packet		*/
#define	DC_DISC		0010000		/* Recv: discard packet		*/
#define	DC_RUNT		0004000		/* Recv: runt packet (too short)*/
#define	DC_HLEN		0003400		/* Recv: bits 10:8 of rec. len.	*/
#define	DC_LLEN		0000377		/* Recv: bits  7:0 of rec. len.	*/
#define	DC_XLEN		60		/* manual says add 60 to len...	*/
#define	DC_RRES		0000370		/* Recv: vendor reserved	*/
#define	DC_FRAM		0000004		/* Recv: frame alignment	*/
#define	DC_CERR		0000002		/* Recv: CRC error		*/
#define	DC_OVER		0000001		/* Recv: overflow error		*/

/* Definitions of device setup packet (see manual for details) */

#define	DQ_ROWS		16		/* setup has 16 rows		*/
#define	DQ_COLS		8		/* setup has 8 columns		*/
#define	DQ_SETD		8		/* displacement of 2nd copy	*/
#define	DQ_BCST		0377		/* broadcast address bytes	*/

#define	dqlen(x) (-(((x)+1)>>1))	/* buffer len: map byte length	*/
					/* to 2s compl. of word length	*/

#define	DQ_RTO		300		/* maximum time to wait for read*/

struct	dqsetu	{			/* deqna setup packet strructure*/
	char 	dq_set[DQ_ROWS][DQ_COLS];/* holds one setup packet	*/
	char	dq_echo[DQ_ROWS][DQ_COLS];/* holds echo of packet	*/
	char	dq_crc[4];		/* allow deqna extra 4 bytes	*/
};
