/* mem.h - freestk, roundew, truncew */

/*----------------------------------------------------------------------
 * roundew, truncew - round or truncate address to next even word
 *----------------------------------------------------------------------
 */
#define	roundew(x)	(int *)( (3 + (int)(x)) & (~3) )
#define	truncew(x)	(int *)( ((int)(x)) & (~3) )

/*----------------------------------------------------------------------
 *  freestk  --  free stack memory allocated by getstk
 *----------------------------------------------------------------------
 */
#define freestk(p,len)	freemem((unsigned)(p)			\
				- (unsigned)(roundew(len))	\
				+ (unsigned)sizeof(int),	\
				roundew(len) )

struct	mblock	{
	struct	mblock	*mnext;
	unsigned mlen;
	};
extern	struct	mblock	memlist;	/* head of free memory list	*/
extern	int	*maxaddr;		/* max memory address		*/
extern	int	etext;			/* address beyond text segment	*/
extern	int	edata;			/* address beyond data segment	*/
extern	int	end;			/* address beyond loaded memory	*/

