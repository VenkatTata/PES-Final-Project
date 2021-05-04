/*
 * touch.h
 *
 * Created on: 06-Mar-2021
 * Author: Venkat Sai Krishna Tata
 */

#ifndef TOUCH_H_
#define TOUCH_H_

/*INCLUDES*/
#include <stdint.h>
//#include "fsl_debug_console.h"
extern volatile uint32_t touch_val;
/*MACROS*/
#define TOUCH_OFFSET 	(570)	  	//Offset for scan range
#define TOUCH_DATA 		(TSI0->DATA & 0xFFFF) // Accessing TSI0_DATA_TSICNT of TSI0_DATA
#define MIN_INPUT		(60)		//Minimum THreshold for TSI touch

/*FUNCTION PROTOTYPES*/
/*
 * @Name		init_TSI
 * @Description	Initializes the Touch Sensing Input module by enabling it in low power mode
 * 				and set as mode 0 (capacitive sensing mode) with scanning the electrode 32 times
 *
 * @parameters	None
 *
 * @Returns		None
 */
void init_TSI(void);

 /*
  * @Name		TSI_Scan_LH
  * @Description	Reads the TSI Module from low to high capacitance
  *
  * @parameters	None
  *
  * @Returns		The value read from the TSI module, subtracted by the offset to receive
  * 				values 1 or 2 when not touched
  */
uint16_t TSI_Scan_LH(void);

#endif /* TOUCH_H_ */
