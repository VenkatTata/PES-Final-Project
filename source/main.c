/*
 * main.c - application entry point
 * 
 * Author : Venkat Sai Krishna Tata
 * Reference: Howdy Pierce, howdy.pierce@colorado.edu
 */

#include "sysclock.h"
#include "UART.h"
#include "UI.h"
#include "test_buffer.h"
#include "LEDs.h"
#include "i2c.h"
#include "extra_switch.h"
#include "touch.h"
#include "test_mma.h"
#include "mma8451.h"
#include "MKL25Z4.h"

int main(void)
{
	//Initialize the system clock
	sysclock_init();
	//Initialise the UART0 module
	Init_UART0();
	//Test the buffer if in DEBUG mode only
	init_I2C();

	Init_RGB_LEDs();
#ifdef DEBUG
		test_cbfifo();
		test_accelerometer();
	#endif
	if (!init_MMA()) {
		Control_RGB_LEDs(1, 0, 0);
		while (1)
			;
	}

	init_TSI();
	init_switch();

	//Begins the command interface UI
	command_interface();
}


