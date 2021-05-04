/*
 * I2C.h
 *
 * Created on: 01-May-2021
 * Author: Venkat Sai Krishna Tata
 */

//INCLUDES
#include <stdint.h>

/* public function prototypes*/

/*
 * @Name		init_I2C
 * @Description	Initializes the I2C0 peripheral wired to the accelerometer from the KL25Z
 * 				pins of 24 and 25. The pre-scaler is set to enable the I2C module operate with
 * 				the standard I2C mode (in 100kHz). KL25Z is set as master and the communication
 * 				is enabled
 *
 * @parameters	none
 * @Returns		none
 */
void init_I2C();
/*
 * @Name		i2c_start_seq
 * @Description	Function implements the sequence of initiating the transmission and generating
 * 				a start bit together
 *
 * @parameters	none
 * @Returns		none
 */
void i2c_start_seq();

/*
 * @Name		i2c_read_addr
 * @Description	Sets up the sequence of activities required for a read operation which include
 * 				writing to the device an address from which the Master wants to read and
 * 				generating a repeated start followed by a read condition with the LSB set. Following
 * 				this activities, any number of bytes can be read sequentially.
 *
 * @parameters	uint8_t , uint8_t - device address and location of the data to be read from
 * @Returns		none
 */
void i2c_read_addr(uint8_t dev_addr, uint8_t location);

/*
 * @Name		i2c_read_reg
 * @Description	Performs the sequence of activities to read a set of register values from the
 * 				accelerometer. Since the data sent by the slave accelerometer is auto incremented
 * 				, bytes can be read as long as the desired registers are consecutive
 *
 * @parameters	uint8_t - whether to transmit ack or nack on byte reception
 * @Returns		uint8_t - the byte received from the slave
 */
uint8_t i2c_read_reg(uint8_t tx_ack);

/*
 * @Name		i2c_ack_wait
 * @Description	The Interrupt Flag is set when the transfer, including the ACK/NACK is complete
 * 				or when slave address matches indicating the presence of slave. Note: Though the
 * 				operation of I2C is not in interrupt mode, IICIF flag is set and more reliable than
 * 				the TCF Transfer complete flag (source : NXP support)
 *
 * @parameters	none
 * @Returns		none
 */
void i2c_ack_wait();

/*
 * @Name		i2c_rxByte
 * @Description	Performs the sequence of activities to read a single byte from the slave. The sequence
 * 				includes transmitting the device address, then data location and restarting the operation
 * 				by sending a repeated start. Again, on sending the device address, master retrieves data
 *
 * @parameters	uint8_t,uint8_t - The slave address and the location of the data in slave register map
 * @Returns		uint8_t - the byte received from the slave
 */
uint8_t i2c_rxByte(uint8_t dev_addr, uint8_t location);

/*
 * @Name		i2c_txByte
 * @Description	Performs the sequence of activities to write a single byte to the slave. The sequence
 * 				includes transmitting the device address, then register location and then the byte value
 * 				to be set in that register.
 *
 * @parameters	uint8_t,uint8_t,uint8_t - The slave address and the control register address to which the
 * 								the data or configuration parameters are to be written
 *
 * @Returns		none
 */
void i2c_txByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t txbyte);
