/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

#include <stdio.h>

#include	"gsm.h"
#include	"gsm610_priv.h"

/*
 *  4.3 FIXED POINT IMPLEMENTATION OF THE RPE-LTP DECODER
 */

attrinline void Postprocessing (
	struct gsm_state	* S,
	register word 		* s)
{
	register int		k;
	register word		msr = S->msr;
	register word		tmp;

	for (k = 160; k--; s++) {
		tmp = GSM_MULT_R( msr, 28180 );
		msr = GSM_ADD(*s, tmp);  	   /* Deemphasis 	     */
		*s  = GSM_ADD(msr, msr) & 0xFFF8;  /* Truncation & Upscaling */
	}
	S->msr = msr;
}

void Gsm_Decoder (
	struct gsm_state	* S,

	word		* LARcr,	/* [0..7]		IN	*/

	word		* Ncr,		/* [0..3] 		IN 	*/
	word		* bcr,		/* [0..3]		IN	*/
	word		* Mcr,		/* [0..3] 		IN 	*/
	word		* xmaxcr,	/* [0..3]		IN 	*/
	word		* xMcr,		/* [0..13*4]		IN	*/

	word		* s)		/* [0..159]		OUT 	*/
{
	int		j;
//	static word		erp[40], wt[160];
	word		erp[40], wt[160];
	word		* drp = S->dp0 + 120;
	
	
	// 86ms
	
	for (j=0; j <= 3; j++, xmaxcr++, bcr++, Ncr++, Mcr++, xMcr += 13) {

		Gsm_RPE_Decoding( /*-S,-*/ *xmaxcr, *Mcr, xMcr, erp );
		Gsm_Long_Term_Synthesis_Filtering( S, *Ncr, *bcr, erp, drp );
		
		int k;
		for (k = 0; k <= 39; k++) wt[ j * 40 + k ] =  drp[ k ];
		
//		rMemCopy32DMA3(&drp[0],&wt[j*40],40*sizeof(word));
	}

	Gsm_Short_Term_Synthesis_Filter( S, LARcr, wt, s ); // 527ms
	Postprocessing(S, s); // 35ms
}
/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch 
** revision control system.
**
** arch-tag: 11ae5b90-2e8b-400b-ac64-a69a1fc6cc41
*/

