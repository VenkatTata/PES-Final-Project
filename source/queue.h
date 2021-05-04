
#ifndef QUEUE_H
#define QUEUE_H
#include <string.h>
#include <stdint.h>
#include <MKL25Z4.h>

#define SIZE (256)

typedef struct {
  unsigned int write; // Index of oldest data element
  unsigned int read; // Index of next free space
  unsigned int length; // Number of elements in use
  uint8_t cbfifo_buf[SIZE];
} cbfifo_t;


/*
 * @Name		cbfifo_init
 * @Description	Initializes the buffers entered as argument with initial values
 *
 * @parameters	cbfifo_t * cb
 *
 * @Returns		None
 */
extern void cbfifo_init(cbfifo_t * cb);

/*
 * cbfifo_length: Returns the number of bytes currently on the FIFO.
 *                  Removed data will be copied into the buffer (consumer) pointed to by buf
 *
 * Parameters: None
 *
 * Returns: Number of bytes currently available to be dequeued from the FIFO
*/
extern int cbfifo_length(cbfifo_t * cb);

/*
 * cbfifo_enqueue: Enqueues data onto the FIFO, up to the limit of the available FIFO capacity
 *					number of elements to be enqueued specified by nbytes
 * Parameters: cbfifo_t * cb The structure of the Queue/Buffer to be enqeued to
 *              buf      	Pointer to the data
 *              nbyte    	Max number of bytes to enqueue
 *
 * Returns: The number of bytes actually enqueued, which could also be 0. But, -1 in case of an error
 * Possible errors: Trying to enqueu while the buffer/FIFO is full
*/
extern int cbfifo_enqueue(cbfifo_t * cb, void* d,size_t nbytes);

/*
 * cbfifo_dequeue: Dequeues/ remove up to nbyte bytes of data from the FIFO
 *                  Removed data will be copied into the buffer (consumer) pointed to by buf
 *
 * Parameters:
 * 				cbfifo_t * cb The buffer to which the dequeued bytes are stored
 *              buf      Destination for the dequeued data
 *              nbyte    Bytes of data requested
 *
 * Returns: The number of bytes actually copied, which will be between 0 and nbyte
 *          -1, if the buffer is NULL
*/
extern size_t cbfifo_dequeue(cbfifo_t * cb,void* d,size_t nbytes);

/*
 * cbfifo_capacity: Returns the FIFO's capacity
 *
 * Parameters: None
 *
 * Returns: The capacity, in bytes, for the FIFO
*/
extern size_t cbfifo_capacity();
#endif // QUEUE_H
