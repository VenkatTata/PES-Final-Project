/**
 * @file    mma8451.c
 * @brief   Initializes the mma8451 accelerometer integrated on the FRDM KL25Z board.
 * 			The accelerometer is set to work in normal and active mode. The measured
 * 			The 14-bit output data is read from the accelerometer registers and converted
 * 			to angular values
 *
 * @author	Venkat Sai Krishna Tata
 * @Reference Alexander Dean, Textbook- Chapter 8 Serial Communication
 * 			Textbook: Embedded Systems Fundamentals with ARM Cortex-M based Microcontrollers
 * @Date	05/02/2021
 */

//INCLUDES
#include <math.h>
#include <stdint.h>
#include "i2c.h"
#include "mma8451.h"
#include "MKL25Z4.h"

//MACROS
#define MMA_DEV_ADDR (0x3A)
#define REG_OUT_Y_MSB 0x03
#define CTRL_REG1  0x2A
#define ANGLE_CONV (180)
#define PI_NUM (22)
#define PI_DEN (7)
#define MSB_SHIFT (8)
#define ADJUST_OUT (2)
#define OUT_MSB_Y (0)
#define OUT_LSB_Y (1)
#define OUT_MSB_Z (2)
#define OUT_LSB_Z (3)
#define SENSITIVITY (4096.0)
#define SET_MMA_ACTIVE (0x01)
#define TOTAL_AXIS_BYTES (4)
#define LAST_AXIS_BYTE (2)
#define SET (1)
#define RESET (0)
#define ACK_RXD (0)
#define INIT_SUCCESS (1)
#define INIT_FAILURE (0)

/*
 * See documentation in .h file
 */
int init_MMA()
{
	//Initialize the accelerometer in active mode, with output data rate at 800 Hz
	i2c_txByte(MMA_DEV_ADDR, CTRL_REG1, SET_MMA_ACTIVE);

	//On successful acknowledge received from I2C device, the registers are set with
	//the value and hence initialization complete; lack of ACK means initialization failure
	if((I2C0->S & I2C_S_RXAK_MASK) ==ACK_RXD)
		return INIT_SUCCESS;
	else
		return INIT_FAILURE;
}

/*
 * See documentation in .h file
 */
int compute_angle()
{
	//variables to read axis value, convert them to 14-bit values and convert
	//from radian to angles
	int angle=RESET,ack=SET;
	uint8_t axis_value[TOTAL_AXIS_BYTES];
	int16_t y_axis_read,z_axis_read;
	float radians_y,radians_z;

	//Initiate the start sequence and initialize to read register with address 0x03
	//which corresponds to the Y-AXIS MSB output as per accelerometer register map
	i2c_start_seq();
	i2c_read_addr(MMA_DEV_ADDR, REG_OUT_Y_MSB );
	
	//Read total of 4 bytes starting with Most significant Byte value of y-axis until least
	//significant byte of z-axis which corresponds to reading the real-time 14 bit sample
	//output values of y and z axis
	for( int i=RESET; i<TOTAL_AXIS_BYTES; i++)
	{
		//Read register and master transmits ACK on reception since nack value is not yet set
		axis_value[i] = i2c_read_reg(ack);

		//For the last byte to be read, setting of ack indicates NACK is sent to convey end of read
		if(i==LAST_AXIS_BYTE)
			ack=RESET;
	}

	//Appending MSB to LSB of y and z axis measurements and adjusting 16 bit values to the valid
	//14 bit output values by shifting the least significant 2 bits which are always 0
	y_axis_read=(((int16_t) ((axis_value[OUT_MSB_Y]<<MSB_SHIFT) | axis_value[OUT_LSB_Y]))>>ADJUST_OUT);
	z_axis_read=(((int16_t) ((axis_value[OUT_MSB_Z]<<MSB_SHIFT) | axis_value[OUT_LSB_Z]))>>ADJUST_OUT);

	//The digital sensitivity of MMA8451 in 2g mode is set to 4096 counts/g as per specifications
	//The acceleration in g-force measurement is hence output values per g or sensitivity
	radians_y = y_axis_read/SENSITIVITY;
	radians_z = z_axis_read/SENSITIVITY;

	//inverse tan math function evaluates the quadrant in which the angle is present
	//The resultant value is in radian, hence converted to angle and returned
	angle = atan2(radians_y, radians_z)*ANGLE_CONV*PI_DEN/PI_NUM;
	return angle;
}

