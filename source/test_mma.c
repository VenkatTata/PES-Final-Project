/*
 * test_mma.c
 *
 *  Created on: 02-May-2021
 *      Author: saikr
 */

#include "test_mma.h"
#include "i2c.h"
#include "MKL25Z4.h"
#include <assert.h>
#include <stdio.h>
#define NO_ACK_RXD 1
#define ACK_RXD 0
#define INCOMP 0
#define MMA_ADDR 0x3A
#define DUMMY_ADDR 0x20
#define MMA
#define TX_DONE I2C0->S & I2C_S_IICIF_MASK
#define SLAVE_ACK I2C0->S & I2C_S_RXAK_MASK
#define REG_WHOAMI 0x0D
#define DEV_ID 0x1A
void test_accelerometer()
{
	int g_total_test=0,g_total_test_pass=0;
//	g_total_test++;
//		i2c_start_seq();
//		if(i2c_rxByte(0x00 ,REG_WHOAMI)==0xFF)
//			g_total_test_pass++;
//		for(int i=0;i<4;i++);
	g_total_test++;
	i2c_start_seq();
	if(i2c_rxByte(MMA_ADDR ,REG_WHOAMI)==DEV_ID)
		g_total_test_pass++;
	for(int i=0;i<4;i++);

//	g_total_test++;
//	i2c_start_seq();
//	I2C0->D = MMA_ADDR;
//	while((TX_DONE) == INCOMP);
//	if((I2C0->S & I2C_S_RXAK_MASK) ==ACK_RXD)
//		g_total_test_pass++;
//	g_total_test++;
//	I2C0->C1 &= ~I2C_C1_MST_MASK;
//	for(int i=0;i<4;i++);
//	i2c_start_seq();
//	I2C0->D = DUMMY_ADDR;
//	while((TX_DONE) == INCOMP);
//	if((I2C0->S & I2C_S_RXAK_MASK) ==NO_ACK_RXD)
//		g_total_test_pass++;

	for(int i=0;i<4;i++);
//	I2C0->C1 &= ~I2C_C1_MST_MASK;

	printf("Accelerometer Implementation : passed %d/%d test cases\n\r",g_total_test_pass,g_total_test);
}

