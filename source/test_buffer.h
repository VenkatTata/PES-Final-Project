/*
 * test_buffer.h
 *
 *  Created on: 04-Apr-2021
 *      Author: saikr
 */

#ifndef TEST_BUFFER_H_
#define TEST_BUFFER_H_
#include <string.h>
#include <assert.h>
#include "UART.h"
#include "queue.h"
/*
 * @Name		test_cbfifo
 * @Description	Performs tests on the cbfifo enqueue, dequeue,length and capacity functionalities
 *
 * @parameters	None
 *
 * @Returns		None
 */
void test_cbfifo();

#endif /* TEST_BUFFER_H_ */
