/*
 * mma8451.h
 *
 * Created on: 01-May-2021
 * Author: Venkat Sai Krishna Tata
 */
#ifndef MMA8451_H
#define MMA8451_H

/*
 * @Name		init_MMA
 * @Description	Initializes the MMA with 800 Hz as the Output data rate and the sets the accelerometer
 * 				device in active mode. The device is set in normal mode and function returns 1 on
 * 				successful ACKing by the device
 *
 * @parameters	none
 *
 * @Returns		int-returns the status of initialization
 */
int init_MMA();

/*
 * @Name		compute_angle
 * @Description	Function reads the real-time samples stored in the accelerometer register and converts
 * 				them to valid 14-bit output values and based on the axis locations, the quadrant is
 * 				evaluated. The resultant radian value is converted to angles in degrees and returned.
 *
 *
 * @parameters	none
 *
 * @Returns		int- current orientation of the device( angles in degree)
 */
int compute_angle();


#endif
