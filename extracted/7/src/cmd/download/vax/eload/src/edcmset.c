/* edcmset.c -- edcmset */

#include "../h/eload.h"
#include "../h/deqna.h"

/*------------------------------------------------------------------------
 *  edcmset -- initialize last command in deqna command list
 *------------------------------------------------------------------------
*/
edcmset(dcmptr)
struct	dcmd	*dcmptr;
{
	dcmptr->dc_flag = DC_NUSED;
	dcmptr->dc_bufh = DC_ENDL;
	dcmptr->dc_buf =  (short) NULL;
	dcmptr->dc_st1 =  dcmptr->dc_st2 = DC_INIT;
}
