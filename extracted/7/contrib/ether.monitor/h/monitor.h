/* monitor.h */

/* This file is a address-name or number-name reference table */



struct	name	{
	char	*n_epaddr;
	char	*n_ipaddr;
	char	*n_nam;
};

#define ZERO	"unknown"
#define	EAZERO	"0000.0000.0000"
#define	IAZERO	"0.0.0.0"
#define	IPRO_TCP	6

#define	TABLE	"0800.2b03.0ee3",	"128.10.2.1",	"arthur",	\
		"aa00.0301.27c0",	"128.10.2.7",	"blays",	\
		"0207.0100.2fca",	"128.10.2.10",	"ector",	\
		"0800.1100.834d",	"128.10.2.74",	"elaine",	\
		"aa00.0301.25b9",	"128.10.2.51",	"excalibur",	\
		"0800.2b02.0005",	"128.10.2.8",	"gwen",		\
		"0800.2001.47eb",	"128.10.2.26",	"lancelot",	\
		"0800.1100.82fa",	"128.10.2.71",	"lot",		\
		"0800.2001.0756",	"128.10.2.20",	"lucas",	\
		"0800.2b02.2618",	"128.10.2.3",	"merlin",	\
		"0207.0100.27ba",	"128.10.2.2",	"mordred",	\
		"0800.2b02.898a",	"128.10.2.50",	"morgana",	\
		"00dd.00a8.9c00",	"128.10.2.6",	"myrddin",	\
		EAZERO,			"128.10.2.9",	"pellinore",	\
		"aa00.0301.3d49",	"128.10.2.5",	"pendragon",	\
		"0800.2001.6797",	"128.10.2.28",	"percival",	\
		"0800.2001.87ad",	"128.10.2.56",	"tristram",	\
		EAZERO,			"128.10.2.83",	"wizard1",	\
		EAZERO,			"128.10.2.30",	"xinu",		\
		"aa00.0301.31c9",	"128.10.2.31",	"knight1",	\
		"aa00.0301.1344",	"128.10.2.32",	"knight2",	\
		"aa00.0301.1346",	"128.10.2.33",	"knight3",	\
		"aa00.0301.12a5",	"128.10.2.34",	"knight4",	\
		EAZERO,			"128.10.2.35",	"knight5",	\
		"aa00.0301.37ef",	"128.10.2.36",	"knight6",	\
		"aa00.0301.3bed",	"128.10.2.37",	"knight7",	\
		"0800.2b03.1af3",	"128.10.2.38",	"knight8",	\
		"0800.2001.608d",	"128.10.2.40",	"lionel",	\
		"0800.4c00.2044",	"128.10.2.73",	"annex",	\
		EAZERO,			"0.0.0.0",	"unknown"


struct	sign	{
	unsigned	s_number;
	char		*s_name;
	char		*s_lname;
};
#define EPS	0x200,	"pup",		"EP_PUP",	\
		0x600,	"xns",		"EP_XNS",	\
		0x800,	"ip",		"EP_IP",	\
		0x804,	"chaos",	"EP_CHAOS",	\
		0x806,	"arp",		"EP_ARP",	\
		0x8035,	"rarp",		"EP_RARP",	\
		-1,	0,		"UNKNOWN"

#define IPS	1,	"icmp",		"IPRO_ICMP",	\
		3,	"ggp",		"IPRO_GGP",	\
		6,	"tcp",		"IPRO_TCP",	\
		8,	"egp",		"IPRO_EGP",	\
		9,	"igp",		"IPRO_IGP",	\
		17,	"udp",		"IPRO_UDP",	\
		66,	"rvd",		"IPRO_RVD",	\
		-1,	0,		"UNKNOWN"

#define	ARPS	1,	"req",		"AR_REQ",	\
		2,	"rply",		"AR_RPLY",	\
		3,	"rreq",		"AR_RREQ",	\
		4,	"rrly",		"AR_RRLY",	\
		-1,	0,		"UNKNOWN"

#define UDPS	7,	"uecho",	"UECHO",	\
		9,	"udiscard",	"UDISCARD",	\
		11,	"uusers",	"UUSERS",	\
		13,	"udatime",	"UDATIME",	\
		17,	"ugodt",	"UGODT",	\
		19,	"ucharhen",	"UCHARGEN",	\
		37,	"utime",	"UTIME",	\
		43,	"uwhois",	"UWHOIS",	\
		53,	"udname",	"UDNAME",	\
		69,	"utftp",	"UTFTP",	\
		513,	"urwho",	"URWHO",	\
		-1,	0,		"UNKNOWN"


#define	ICMPS	0x000,	"icrpech",	"ICRPECH",		\
		0x300,	"icdestu",	"ICDESTU",		\
		0x301,	"ichostu",	"ICHOSTU",		\
		0x302,	"icprotu",	"ICPROTU",		\
		0x303,	"icportu",	"ICPORTU",		\
		0x304,	"icfragn",	"ICFRAGN",		\
		0x305,	"icsrcrf",	"ICSRCRF",		\
		0x400,	"icsrcqn",	"ICSRCQN",		\
		0x500,	"icredir",	"ICREDIR",		\
		0x501,	"ichostr",	"ICHOSTR",		\
		0x502,	"ictnetr",	"ICTNETR",		\
		0x503,	"icthosr",	"ICTHOSR",		\
		0x800,	"icrqech",	"ICRQECH",		\
		0xb00,	"ictimex",	"ICTIMEX",		\
		0xb01,	"icfratx",	"ICFRATX",		\
		0xc00,	"icparmp",	"ICPARMP",		\
		0xd00,	"ictimst",	"ICTIMST",		\
		0xe00,	"ictimrp",	"ICTIMRP",		\
		0xf00,	"icrqinf",	"ICRQINF",		\
		0x1000,	"icrpinf",	"ICRPINF",		\
		-1,	0,			"UNKNOWN"

#define	TCPS	7,	"echo",		"TCPECHO",		\
		9,	"discard",	"TCPDISCARD",		\
		11,	"systat",	"TCPSYSTAT",		\
		13,	"daytime",	"TCPDAYTIME",		\
		15,	"netstat",	"TCPNETSTAT",		\
		17,	"quotd",	"TCPQUOTD",		\
		19,	"chargen",	"TCPCHARGEN",		\
		21,	"ftp",		"TCPFTP",		\
		23,	"telnet",	"TCPTELNET",		\
		25,	"smtp",		"TCPSMTP",		\
		37,	"time",		"TCPTIME",		\
		42,	"name",		"TCPNAME",		\
		43,	"whois",	"TCPWHOIS",		\
		53,	"domain",	"TCPDOMAIN",		\
		57,	"mtp",		"TCPMTP",		\
		77,	"rje",		"TCPRJE",		\
		79,	"finger",	"TCPFINGER",		\
		87,	"ttylink",	"TCPTTYLINK",		\
		95,	"supdup",	"TCPSUPDUP",		\
		101,	"hostname",	"TCPHOSTNAME",		\
		115,	"write",	"TCPWRITE",		\
		512,	"rexec",	"TCPREXEC",		\
		513,	"rlogin",	"TCPRLOGIN",		\
		514,	"rsh",		"TCPRSH",		\
		-1,	0,		"UNKNOWN"

extern	struct	name	table[];
extern	struct	sign	ep_tab[];
extern	struct	sign	arp_tab[];
extern	struct	sign	ip_tab[];
extern	struct	sign	udp_tab[];
extern	struct	sign	tcp_tab[];
extern	struct	sign	icmp_tab[];
