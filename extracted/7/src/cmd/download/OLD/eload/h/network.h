/* network.h */

/* Declarations data conversion and checksum routines */

extern	short		hs2net();	/* host to network short	*/
extern	short		net2hs();	/* network to host short	*/
extern	long		hl2net();	/* host-to-network long		*/
extern	long		net2hl();	/* network to host long		*/
extern	long		hl2vax();	/* pdp11-to-vax long		*/
extern	long		vax2hl();	/* vax-to-pdp11 long		*/
extern	short		cksum();	/* 1s comp of 16-bit 2s comp sum*/
