
/* dscopy.c - dscopy */

/*------------------------------------------------------------------------
 *  dscopy  --  copy a portion of memory (usually to/from the Multi-bus)
 *------------------------------------------------------------------------
 */
dscopy(from, to, length)
	char	*from;
	char	*to;
	int	length;
{
#ifdef	DEBUG
	dotrace("dscopy", &from, 3);
#endif
	for(; length>0; length--)
		*to++ = *from++;
}
