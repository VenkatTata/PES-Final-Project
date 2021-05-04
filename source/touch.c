/**
 * @file    Touch.c
 * @brief   Configuring for Touch Sensing Input (TSI Module) to scan electrode 32 times
 * 			using channel 10 of TSI
 *
 * @author	Venkat Sai Krishna Tata
 * @Date	03/06/2021
 */

#include "touch.h"
#include <MKL25Z4.h>
volatile uint32_t touch_val=0;
void init_TSI(void)
{
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;

/*
	TSI0->GENCS =   TSI_GENCS_OUTRGF_MASK	|// Out of range flag
					TSI_GENCS_MODE(0u)		|// Mode 0 - capacitive sensing mode
					TSI_GENCS_REFCHRG(0u)	|// reference oscillator charge and discharge value is 500nA
					TSI_GENCS_DVOLT(0u) 	|// oscillator voltage rails set to default
					TSI_GENCS_EXTCHRG(0u) 	|// electrode oscillator charge and discharge value is 500nA
					TSI_GENCS_PS(0u) 		|// frequency clock divided by one
					TSI_GENCS_NSCN(31u) 	|// scanning the electrode 32 times
					TSI_GENCS_TSIEN_MASK 	|// enabling the TSI module
					TSI_GENCS_STPE_MASK 	|// Enabling TSI module in low power mode
					TSI_GENCS_EOSF_MASK ; 	 // writing one to clear the end of scan flag
*/

	TSI0->GENCS = TSI_GENCS_MODE(0u) |
								TSI_GENCS_REFCHRG(0u) |
								TSI_GENCS_DVOLT(0u) |
								TSI_GENCS_EXTCHRG(0u) |
								TSI_GENCS_PS(0u) |
								TSI_GENCS_NSCN(31u) |
								TSI_GENCS_TSIEN_MASK |
								//TSI_GENCS_STPE_MASK |  // enabling the TSI in low power modes
								TSI_GENCS_EOSF_MASK |
								TSI_GENCS_ESOR_MASK | //enabling interrupt using end of scan
								TSI_GENCS_TSIIEN_MASK; //enabling the TSI interrupt


	TSI0->DATA = 	TSI_DATA_TSICH(10u); // selecting channel 10
	//enaling interrupt in NVIC
	NVIC_SetPriority(TSI0_IRQn, 2);
	NVIC_ClearPendingIRQ(TSI0_IRQn);
	NVIC_EnableIRQ(TSI0_IRQn);
	// Allow low leakage stop mode
	SMC->PMPROT = SMC_PMPROT_ALLS_MASK; //
	// Enable low-leakage stop mode and regular run mode
	SMC->PMCTRL = SMC_PMCTRL_STOPM(3) | SMC_PMCTRL_RUNM(0);
	SMC->STOPCTRL = SMC_STOPCTRL_PSTOPO(0) | SMC_STOPCTRL_VLLSM(3);
	// Enable LLWU
	// allow TSI0 to wake LLWU
//	LLWU->ME |= LLWU_ME_WUME4_MASK;
}
/*uint16_t TSI_Scan_LH(void)
{

	TSI0->DATA 	= 	TSI_DATA_TSICH(10u); 	// Using channel 10 of The TSI
	TSI0->DATA 	|= TSI_DATA_SWTS_MASK; 		// Software trigger for scan
	while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK ));//Waiting until TSI scans 32 times per electrode
	uint32_t scan = TOUCH_DATA;				//Store Scanned data in scan
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK; 	// Clearing End of Scan Flag after all electrodes finish scanning
	return scan - TOUCH_OFFSET;				// Return scan value with subtracted OFFSET
}*/
void TSI0_IRQHandler (void){
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; 	//writing one to clear the flag
	touch_val = TOUCH_DATA - TOUCH_OFFSET;

}
