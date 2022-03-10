/* nserver.h - getInetName, GetInetAddr */

/*
 * Some of the declarations seem cryptic, see RFC #'s 882, 883, 973 
 *
 */


#define PACKETLGTH    128                        /* max message size */
#define N_SERVERADDR  "128.10.2.5:53"            /* name server address */
#define MSG_HEADER    "\0\1\1\0\0\1\0\0\0\0\0\0" /* see RFC's           */
#define QTN_HEADER    "\7in-addr\4arpa\0\0\014\0\1" /* pointer query    */
					     /* (not documented in RFC's) */
#define HEADER_LGTH    12
#define QTNHEADER_LGTH 18  
#define RR_LGTH        10


/*   message header (see references)    */

struct iq {
	unsigned short id;                        /* message id         */
        char opcode;                              /* includes op. code  */
        char rcode : 4;                           /* # of responses     */
        char unused : 4;
	unsigned short qdcount;                   /* # of questions (1) */
	unsigned short ancount;                   /* # of answers       */
	unsigned short nscount;
	unsigned short arcount;
	};



