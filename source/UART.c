/**
 * @file    UART.c
 * @brief   Describes the Implementation of UART serial interface which is based on buffers
 * 			Handles the printf,putchar,getchar,scan functions
 *
 * @author	Venkat Sai Krishna Tata
 * @Date	04/04/2021
 * Reference : Alexander Dean, link mentioned below
 * link: https://github.com/alexander-g-dean/ESF/blob/master/NXP/Code/Chapter_8/Serial-Demo/src/UART.c
 */
#include "UART.h"
#include "sysclock.h"
#include "queue.h"
#include "MKL25Z4.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//Defines can be modified to change the UART settings
//Disabling nine bits data sizes, implies 8 bit data mode
//Enable or disable paramaters by swapping enable or disable macros
#define NINE_BITS_DATA_SIZE 	(DISABLE)
#define BAUD_RATE 				(38400)
#define PARITY 					(DISABLE)
#define TWO_STOP_BITS 			(ENABLE)
#define UART_OVERSAMPLE_RATE 	(15)
#define BUS_CLOCK 				(24e6)
#define DISABLE (0)
#define ENABLE (1)

//Creates two instances of cbfifo which act as the reciever and transmit buffers
cbfifo_t TxQ, RxQ;

//Function which handles the printf,putchar function. Glue function between UART and standard library calls
int __sys_write(int handle, char* buf, int count)
{
	//Check if buffer is NULL
	if(buf == NULL)
	{
		return -1;
	}
	//Enqueue until last element of the string
	while (*buf != '\0')
	{
		//Check if Transmit buffer is full
		while (cbfifo_length(&TxQ)==SIZE)
		{
			// wait for space to open up
			UART0->C2 |= UART0_C2_TIE(1);
		}
		//Enqueue element if space present
		cbfifo_enqueue(&TxQ, buf,1);
		//Increment the pointer to the string
		buf++;
	}
	//Enable transmitter by enabling the interrupt to begin transmit to Terminal from tx buffer
	if(!(UART0->C2 & UART0_C2_TIE_MASK))
	{
		UART0->C2 |= UART0_C2_TIE(1);
	}
	return 0;
}

//Acts as a glue function between read actions
int __sys_readc(void)
{
	//If the length of receiver buffer is not empty, dequeue element and returns it
	uint8_t byte;
	while (cbfifo_length(&RxQ) == 0);
	cbfifo_dequeue(&RxQ,&byte,1);
	return byte;
}


/*
 * Initializing the UART for BAUD_RATE: 38400, Data Size: 8, Parity: None, Stop Bits: 2
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   void
 */
void Init_UART0() {
	uint16_t sbr;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 24 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((BUS_CLOCK)/(BAUD_RATE * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Disable interrupts for RX active edge and LIN break detect, select two stop bits
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(TWO_STOP_BITS) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(NINE_BITS_DATA_SIZE) | UART0_C1_PE(PARITY);

	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);

	// Enable interrupts. Listing 8.11 on p. 234
	cbfifo_init(&TxQ);
	cbfifo_init(&RxQ);

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

}




/*
 * UART IRQ_Handler
 *
 * Parameters:
 *   void
 * Returns:
 *   void
 */
void UART0_IRQHandler(void) {

	uint8_t ch,byte;
	//If error during reception , clear the errors and skips the enqueue of noisy input character
	if (UART0->S1 & (UART_S1_OR_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK))
	{
		UART0->S1 |= (UART0_S1_OR_MASK  | UART0_S1_FE_MASK | UART0_S1_PF_MASK);
	}
	//If reciever flag is enabled, character received,enqueue into receive buffer
	if (UART0->S1 & UART0_S1_RDRF_MASK)
	{
		// received a character, ensures it is ASCII character
		ch = ((UART0->D)&0x7F);
		//If buffer not full, enqueue characters received
		if (!(cbfifo_length(&RxQ)==SIZE))
		{
			cbfifo_enqueue(&RxQ, &(ch),1);
		}
		else
		{
			// error - queue full.
			// discard character
		}
	}

	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
				(UART0->S1 & UART0_S1_TDRE_MASK) )
	{ 	// tx buffer empty
		// can send another character
		if (cbfifo_length(&TxQ)==0)
		{
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
		else
		{
			//If buffer not empty, can transmit another character
			cbfifo_dequeue(&TxQ,&byte,1);
			UART0->D = byte;
		}
	}
}

