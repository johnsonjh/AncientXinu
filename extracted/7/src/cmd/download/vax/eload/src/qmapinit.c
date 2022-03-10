/* qmapinit.c -- qmapinit */

#include <qbus.h>

/*------------------------------------------------------------------------
 *  qmapinit -- initialize Microvax II qbus mapping registers
 *		called by eload only if running on UVAXII
 *		warning: only 4 megs of memory are mapped for dma access
 *------------------------------------------------------------------------
 */

qmapinit()
{
	unsigned int	*pqmapreg;
       	unsigned int	i;

	*QMMECSR = 1;		/* enable parity error detection */
	for (pqmapreg = QMAP, i = 0; i < QMNMREG; )
		*pqmapreg++ = QMREGV | i++;
	*QMIPCR = QMENAB;	/* enable bus mapping	*/
}
