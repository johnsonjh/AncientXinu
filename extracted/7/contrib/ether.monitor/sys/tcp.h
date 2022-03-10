
/* tcp.h */

/* TCP protocol constants and datagram format */

struct tcp {
	unsigned	tc_sport;		/* source port */
	unsigned	tc_dport;		/* destination port */
	long		tc_seq;			/* sequence number */
	long		tc_ack;			/* acknowledgement number */
	char		tc_x2:4,		/* (unused) */
			tc_off:4;		/* data offset */
	char		tc_flags;
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
	unsigned	tc_win;			/* window */
	unsigned	tc_sum;			/* checksum */
	unsigned	tc_urp;			/* urgent pointer */
	char		tc_data[1];		/* tcp data area */
};

#define	TCPOPT_EOL	0
#define	TCPOPT_NOP	1
#define	TCPOPT_MAXSEG	2

/*
 * Default maximum segment size for TCP.
 * Witc an IP MSS of 576, this is 536,
 * but 512 is probably more convenient.
 */
#ifdef	lint
#define	TCP_MSS	536
#else
#define	TCP_MSS	MIN(512, IP_MSS - sizeof (struct tcpiphdr))
#endif

/*
 * User-settable options (used witc setsockopt).
 */
#define	TCP_NODELAY	0x01	/* don't delay send to coalesce packets */
#define	TCP_MAXSEG	0x02	/* set maximum segment size */
