/**
 * @file    extra_switch.c
 * @brief   Implementing a physical push-button switch which is operated in interrupt mode
 * 			and used by the program to terminate the angle gauge's currently operation feature
 *
 *
 * @author	Venkat Sai Krishna Tata
 * @Date	05/01/2021
 */

//INCLUDES
#include <stdbool.h>
#include "MKL25Z4.h"
#include "extra_switch.h"

//Global variable for interrupt
bool switch_pressed;

/*
 * See documentation in .h file
 */
void init_switch()
{
	//Clearing and setting the pin 3 of PORT D with alternate function as GPIO
	//Enabling and selecting Pull Up resistor and setting pin direction as input
	//Configuring Interrupt on falling edge and clearing Interrupt Status Flag initially
	//Reference to GPIO pin input configuration : Lecture 10 notes of PES class
	SWITCH_PIN_CTRL_REG &= ~PORT_PCR_MUX_MASK;
	SWITCH_PIN_CTRL_REG |= PORT_PCR_MUX(1);
	SWITCH_PIN_CTRL_REG |= PORT_PCR_PE(1) | PORT_PCR_PS(1) | PORT_PCR_IRQC(FALL_EDGE) | PORT_PCR_ISF(0);
	SWITCH_GPIO_PORT->PDDR &= ~(1 << SWITCH_PIN);

	//Set NVIC priority to 3 for Interrupt number 31 (PORT D) and enable Interrupt request
	NVIC_SetPriority(PORTD_IRQn, PRIORITY);
	NVIC_EnableIRQ(PORTD_IRQn);
}

/*
 * @Name		PORTD_IRQHandler
 * @Description	Handler for the interrupt which sets the flag indicating switch is pressed
 * 				Note : function not static though not called in any other file because otherwise
 * 				unused function warning caused.
 *
 * @parameters	None
 *
 * @Returns		None
 */
void PORTD_IRQHandler(void)
{
	//Disable Interrupts to Port D before servicing the Interrupt
	NVIC_DisableIRQ(PORTD_IRQn);

	//Set switch_pressed flag if Interrupt Set Flag is 1 and clear the ISF flag
	//Note : Access to ISFR corresponding location or ISF of corresponding PORT are the same
	switch_pressed=PORTD->ISFR & (1 << SWITCH_PIN);

	//Clear the ISF flag to receive further Interrupts
	//Enable Interrupts before exiting ISR
	SWITCH_PIN_CTRL_REG |=PORT_PCR_ISF(0);
	NVIC_EnableIRQ(PORTD_IRQn);
}
