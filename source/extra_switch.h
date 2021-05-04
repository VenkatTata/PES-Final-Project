/*
 * extra_switch.h
 *
 * Created on: 07-Mar-2021
 * Author: Venkat Sai Krishna Tata
 * References : Lecture 10 notes of Principles of Embedded Software by Howdy Pierce, howdy@cardinalpeak.com
 */

#ifndef EXTRA_SWITCH_H_
#define EXTRA_SWITCH_H_
#include <stdbool.h>

/*MACROS*/
#define SWITCH_GPIO_PORT GPIOD
#define SWITCH_PIN (3)
#define SWITCH_PIN_CTRL_REG PORTD->PCR[SWITCH_PIN]
#define SWITCH_SCGC5_MASK SIM_SCGC5_PORTD_MASK
#define PRIORITY (3)
#define FALL_EDGE (10)

/*VARIABLES*/
extern bool switch_pressed;

/*FUNCTION PROTOTYPES*/

/*
 * @Name		init_switch
 * @Description	Initializes the GPIO pin as input and sets the interrupt capability
 *
 * @parameters	None
 *
 * @Returns		None
 */
void init_switch();

#endif /* EXTRA_SWITCH_H_ */
