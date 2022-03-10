/* startup.s - start */

#define	 ASM
#include <ethdown.h>
#include <procreg.h>
#include <vaxdl.h>

/*----------------------------------------------------------------------------
 *  Eload start location
 *  assumption: processor is in kernel mode on entry 
 *----------------------------------------------------------------------------
*/

	.text
	.globl	estart
estart:					/* boot utility entry pt	*/
	mtpr	$DISABLE, $IPL		/* disable interrupts 		*/
	mtpr	$IURSET, $IUR		/* reset the bus 		*/
	mtpr	$IURUNSET, $IUR		/* unreset the bus		*/
	mtpr	$NOMAP, $MAPEN		/* disable memory management	*/
	mtpr	$intstk, $ISP		/* give interrupt stack memory	*/
					/* we use interrupt stk for all	*/
	mtpr	$MCESRCLR, $MCESR	/* clear machine check history	*/
	mtpr	$0, $SCBB		/* set up for sizmem		*/
	mfpr	$SID, -(sp)		/* see if UVAXII -- if so, must	*/
	bicl2	$SYSTYPE, (sp)		/* set up bus mapping registers	*/
	cmpl	(sp), $UVAXIISID
	jneq	eloadgo
	calls	$0, _qmapinit		/* set up default mapping regs	*/
eloadgo:
	calls	$0, _eload
	jbr	estart			/* should never be executed	*/

	.globl	intstk
	.space	6*1024			/* space for stack		*/
intstk:
	.space	4
