# PES-Final-Project
Code for Final Project of PES, ECEN-5813,Spring 2021

OBJECTIVE
To develop the logic for a digital angle gauge which is based on the MMA8451Q accelerometer included in the FRDM-KL25Z. User can type commands over the serial port and LEDs glow to indicate outputs/measurements. Apart from being used as a digital angle gauge, it can be used as a digital level that works similar to a spirit level tool which is often used to indicate whether a surface is horizontal (level) or vertical (plumb).

FUNCTIONALITY
	If an initialization failure or test case failure exists, red LED glows indicating the error.
On successful initialization, white LED glows only if all automated test cases for I2C protocol and buffers passed. 
The device can then be operated in one of the following four modes based on the user commands entered via the command line interface,
(i) 'measure mode' - displays instantaneous angle measurement values on the command line terminal.
(ii) 'user angle mode' - blue LED glows to indicate that the device is oriented exactly at the angle value input by the user via the command line. (argument for command: angle value)
(iii) 'fixed angle mode'- LED glows with colours purple, brown or cyan to indicate that the device orientation is exactly at 45°,60°, or 90° respectively. 
(iv) ‘level mode’ – green LED indicates that the surface is perfectly level or plumb. (must be calibrated to 0° first)

All of the angular measurements are relative to the 0° setting performed by the user. User does this calibration/setting by tapping the capacitive touch button.
Automated tests
1)	Read over I2C the WHO_AM_I register for device ID and verify with I2Cx_A1 register value which contains the slave address, repeat for cases trying to read from invalid addresses
2)	Check if I2C bus is not busy before or after transfers, which implies other devices can communicate during the meanwhile
3)	Test to exercise the circular buffer FIFO implementation

Manual Tests

1)	Hook up the oscilloscope to verify rise time, fall time of SCL and SDA lines of I2C.
2)	Hook up the logic analyser to perform timing calculations and verify if START, STOP, and DATA VALID conditions for I2C slave (MMA8451Q) timing are satisfied.
3)	[Error Case] If ACK is not received, check if after a timeout, the communication with the peripheral device was re-initiated (check if red LED was glowing to indicate failure).
4)	0° calibrate the device at point A and measure level or plumb on horizontal or vertical surfaces. This can be verified by calibrating the ‘Level’ feature of iPhone’s ‘Measure’ app at point A and measuring level and plumb on the same surfaces.
5)	Tie the FRDM board to the iPhone and measure the angle on the phone (using the ‘Measure’ feature of the ‘Measure’ app) and verify if LED glows with purple, brown or cyan colours for 45°, 60° or 90° tilts. 
6)	Similarly check if green LED glows when device oriented at a user input angular value.
7)	[Corner cases] Change in pitch(y-axis) must not impact the angle/tilt/roll(x-axis) measurements
