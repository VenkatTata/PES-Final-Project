#include "queue.h"

//#define SIZE (256)
//
//typedef struct {
//  unsigned int write; // Index of oldest data element
//  unsigned int read; // Index of next free space
//  unsigned int length; // Number of elements in use
//  uint8_t cbfifo_buf[SIZE];
//} cbfifo_t={.write =0, .read=0,.length=0};

void cbfifo_init(cbfifo_t * cb)
{
	//Set the data structure to all zeroes
  memset(cb->cbfifo_buf,0,SIZE);
  cb->write = 0;
  cb->read = 0;
  cb->length = 0;
}

int cbfifo_enqueue(cbfifo_t * cb, void* d,size_t nbytes)
{
	uint32_t masking_state;
	int enq=0;
	// Check if the Queue is full and if all elements are enqueued
    while((nbytes--) && (cb->length!=SIZE))
	{
    	//Store the element into the buffer
		cb->cbfifo_buf[cb->write] = *(uint8_t*)d++;
		//Update the pointer location upon writing to buffer
		cb->write = ( SIZE-1 ) & ( cb->write+1 );
		// protect q->length++ operation from preemption
		// save current masking state
		masking_state = __get_PRIMASK();
		// disable interrupts
		__disable_irq();
		// update length of buffer
		cb->length++;
		enq++;
		// restore  interrupt masking state
		__set_PRIMASK(masking_state);
    }
    //Return the number of elements enqueued
    return enq;
}

size_t cbfifo_dequeue(cbfifo_t * cb,void* t,size_t nbytes)
{
	uint32_t masking_state;
	size_t deq=0;
	// Check if the Queue is empty and if all elements are deqeued
	while((nbytes--) &&(cb->length!=0))
	{
		//Read data from buffer and store into the destination buffer
		*(uint8_t*)t++ = cb->cbfifo_buf[cb->read];
		//clearing the locations of dequeued elements by inserting 0 since the cbfifo is uint8_t
		cb->cbfifo_buf[cb->read] = 0;
		//Update the read pointer after dequeuing
		cb->read = ( SIZE-1 ) & ( cb->read+1 );
		// protect q->length-- operation from preemption
		// save current masking state
		masking_state = __get_PRIMASK();
		// disable interrupts
		__disable_irq();
		deq++;
		// update variable
		cb->length--;
		// restore  interrupt masking state
		__set_PRIMASK(masking_state);
	}
	//Return number of elements dequeued
  return deq;
}

size_t cbfifo_capacity()
{
    //since the buffer is statically allocated with 256, buffer size is always SIZE (256)
    return SIZE;
}

int cbfifo_length(cbfifo_t * cb)
{
	return cb->length;
}
