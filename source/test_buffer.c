/*
 * test_buffer.c
 *
 *  Created on: 04-Apr-2021
 *  Author: Venkat Sai Krishna Tata
 *      Reference: Howdy Pierce assignment_tester.c
 */

#include "test_buffer.h"
#include <stdio.h>
#define max(x,y) ((x) > (y) ? (x) : (y))

int g_tests_passed = 0;
int g_tests_total = 0;

cbfifo_t test_queue;
#define test_assert(value) {                                            \
  g_tests_total++;                                                      \
  if (value) {                                                          \
    g_tests_passed++;                                                   \
  } else {                                                              \
    printf("ERROR at %d\n\r", __LINE__);               \
  }                                                                     \
}

#define test_equal(value1, value2) {                                    \
  g_tests_total++;                                                      \
  long res1 = (long)(value1);                                           \
  long res2 = (long)(value2);                                           \
  if (res1 == res2) {                                                   \
    g_tests_passed++;                                                   \
  } else {                                                              \
    printf("ERROR: test failure at line %d: %ld != %ld\n\r", __LINE__, res1, res2); \
  }                                                                     \
}
static void
test_cbfifo_one_iteration()
{
	  char *str ="To be, or not to be: that is the question:\n"
		"Whether 'tis nobler in the mind to suffer\n"
		"The slings and arrows of outrageous fortune,\n"
		"Or to take arms against a sea of troubles,\n"
		"And by opposing end them? To die, to sleep--\n"
		"No more--and by a sleep to say we end\n"
		"The heart-ache and the thousand natural shocks\n"
		"That flesh is heir to, 'tis a consummation\n"
		"Devoutly to be wish'd. To die, to sleep;\n"
		"To sleep: perchance to dream: ay, there's the rub;\n"
		"For in that sleep of death what dreams may come\n"
		"When we have shuffled off this mortal coil,\n"
		"Must give us pause.";

  char buf[1024];
  const int cap = cbfifo_capacity();
  size_t ret = 0;

  // the asserts in the following 2 lines are there because they're
  // not testing the student, they're validating that the test is
  // correct
  assert(strlen(str) >= cap*2);
  assert(sizeof(buf) > cap);
  test_assert(cap == SIZE);

  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), 0);
  test_equal(cbfifo_dequeue(&test_queue,buf, 2), 0);

  // enqueue 10 bytes, then dequeue same amt
  test_equal(cbfifo_enqueue(&test_queue,str, 10), 10);
  test_equal(cbfifo_length(&test_queue), 10);
  test_equal(cbfifo_dequeue(&test_queue,buf, 10), 10);
  test_equal(strncmp(buf, str, 10), 0);
  test_equal(cbfifo_length(&test_queue), 0);

  // enqueue 20 bytes;  dequeue 5, then another 20
  test_equal(cbfifo_enqueue(&test_queue,str, 20), 20);
  test_equal(cbfifo_length(&test_queue), 20);
  test_equal(cbfifo_dequeue(&test_queue,buf, 5), 5);
  test_equal(cbfifo_length(&test_queue), 15);
  test_equal(cbfifo_dequeue(&test_queue,buf+5, 20), 15);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str, 20), 0);

  // fill buffer and then read it back out
  test_equal(cbfifo_enqueue(&test_queue,str, cap), cap);
  test_equal(cbfifo_length(&test_queue), cap);
  ret = cbfifo_enqueue(&test_queue,str, 1);
  test_assert(ret == 0 || (int)ret == -1);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str, cap), 0);

  // Add 20 bytes and pull out 18
  test_equal(cbfifo_enqueue(&test_queue,str, 20), 20);
  test_equal(cbfifo_length(&test_queue), 20);
  test_equal(cbfifo_dequeue(&test_queue,buf, 18), 18);
  test_equal(cbfifo_length(&test_queue), 2);
  test_equal(strncmp(buf, str, 18), 0);

  // Now add a bunch of data in 4 chunks
  int chunk_size = (cap-2) / 4;
  for (int i=0; i<4; i++) {
    test_equal(cbfifo_enqueue(&test_queue,str+i*chunk_size, chunk_size), chunk_size);
    test_equal(cbfifo_length(&test_queue), (i+1)*chunk_size + 2);
  }
  test_equal(cbfifo_length(&test_queue), 4*chunk_size + 2);

  // Take out the 2 remaining bytes from above
  test_equal(cbfifo_dequeue(&test_queue,buf, 2), 2);
  test_equal(strncmp(buf, str+18, 2), 0);

  // now read those chunks out a byte at a time
  for (int i=0; i<chunk_size*4; i++) {
    test_equal(cbfifo_dequeue(&test_queue,buf+i, 1), 1);
    test_equal(cbfifo_length(&test_queue), chunk_size*4 - i - 1);
  }
  test_equal(strncmp(buf, str, chunk_size*4), 0);

  // write more than capacity
  test_equal(cbfifo_enqueue(&test_queue,str, 65), 65);
  test_equal(cbfifo_enqueue(&test_queue,str+65, cap), cap-65);
  test_equal(cbfifo_length(&test_queue), cap);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str, cap), 0);

  // write zero bytes
  test_equal(cbfifo_enqueue(&test_queue,str, 0), 0);
  test_equal(cbfifo_length(&test_queue), 0);

  // Exercise the following conditions:
  //    enqueue when read < write:
  //        bytes < CAP-write  (1)
  //        bytes exactly CAP-write  (2)
  //        bytes > CAP-write but < space available (3)
  //        bytes exactly the space available (4)
  //        bytes > space available (5)
  test_equal(cbfifo_enqueue(&test_queue,str, 32), 32);  // advance so that read < write
  test_equal(cbfifo_length(&test_queue), 32);
  test_equal(cbfifo_dequeue(&test_queue,buf, 16), 16);
  test_equal(cbfifo_length(&test_queue), 16);
  test_equal(strncmp(buf, str, 16), 0);

  test_equal(cbfifo_enqueue(&test_queue,str+32, 32), 32);  // (1)
  test_equal(cbfifo_length(&test_queue), 48);
  test_equal(cbfifo_enqueue(&test_queue,str+64, cap-64), cap-64);  // (2)
  test_equal(cbfifo_length(&test_queue), cap-16);
  test_equal(cbfifo_dequeue(&test_queue,buf+16, cap-16), cap-16);
  test_equal(strncmp(buf, str, cap), 0);

  test_equal(cbfifo_enqueue(&test_queue,str, 32), 32);  // advance so that read < write
  test_equal(cbfifo_length(&test_queue), 32);
  test_equal(cbfifo_dequeue(&test_queue,buf, 16), 16);
  test_equal(cbfifo_length(&test_queue), 16);
  test_equal(strncmp(buf, str, 16), 0);

  test_equal(cbfifo_enqueue(&test_queue,str+32, cap-20), cap-20);  // (3)
  test_equal(cbfifo_length(&test_queue), cap-4);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap-8), cap-8);
  test_equal(strncmp(buf, str+16, cap-8), 0);
  test_equal(cbfifo_length(&test_queue), 4);
  test_equal(cbfifo_dequeue(&test_queue,buf, 8), 4);
  test_equal(strncmp(buf, str+16+cap-8, 4), 0);
  test_equal(cbfifo_length(&test_queue), 0);

  test_equal(cbfifo_enqueue(&test_queue,str, 49), 49);  // advance so that read < write
  test_equal(cbfifo_length(&test_queue), 49);
  test_equal(cbfifo_dequeue(&test_queue,buf, 16), 16);
  test_equal(cbfifo_length(&test_queue), 33);
  test_equal(strncmp(buf, str, 16), 0);

  test_equal(cbfifo_enqueue(&test_queue,str+49, cap-33), cap-33);  // (4)
  test_equal(cbfifo_length(&test_queue), cap);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str+16, cap), 0);

  test_equal(cbfifo_enqueue(&test_queue,str, 32), 32);  // advance so that read < write
  test_equal(cbfifo_length(&test_queue), 32);
  test_equal(cbfifo_dequeue(&test_queue,buf, 16), 16);
  test_equal(cbfifo_length(&test_queue), 16);
  test_equal(strncmp(buf, str, 16), 0);

  test_equal(cbfifo_enqueue(&test_queue,str+32, cap), cap-16);  // (5)
  test_equal(cbfifo_dequeue(&test_queue,buf, 1), 1);
  test_equal(cbfifo_length(&test_queue), cap-1);
  test_equal(cbfifo_dequeue(&test_queue,buf+1, cap-1), cap-1);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str+16, cap), 0);

  //    enqueue when write < read:
  //        bytes < read-write (6)
  //        bytes exactly read-write (= the space available) (7)
  //        bytes > space available (8)

  int wpos=0, rpos=0;
  test_equal(cbfifo_enqueue(&test_queue,str, cap-4), cap-4);
  wpos += cap-4;
  test_equal(cbfifo_length(&test_queue), cap-4);
  test_equal(cbfifo_dequeue(&test_queue,buf, 32), 32);
  rpos += 32;
  test_equal(cbfifo_length(&test_queue), cap-36);
  test_equal(strncmp(buf, str, 32), 0);
  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 12), 12);
  wpos += 12;
  test_equal(cbfifo_length(&test_queue), cap-24);

  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 16), 16);  // (6)
  test_equal(cbfifo_length(&test_queue), cap-8);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap-8);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str+rpos, cap-8), 0);

  // reset
  wpos=0;
  rpos=0;
  test_equal(cbfifo_enqueue(&test_queue,str, cap-4), cap-4);
  wpos += cap-4;
  test_equal(cbfifo_length(&test_queue), cap-4);
  test_equal(cbfifo_dequeue(&test_queue,buf, 32), 32);
  rpos += 32;
  test_equal(cbfifo_length(&test_queue), cap-36);
  test_equal(strncmp(buf, str, 32), 0);
  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 12), 12);
  wpos += 12;
  test_equal(cbfifo_length(&test_queue), cap-24);

  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 24), 24);  // (7)
  test_equal(cbfifo_length(&test_queue), cap);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str+rpos, cap), 0);

  // reset
  wpos=0;
  rpos=0;
  test_equal(cbfifo_enqueue(&test_queue,str, cap-4), cap-4);
  wpos += cap-4;
  test_equal(cbfifo_length(&test_queue), cap-4);
  test_equal(cbfifo_dequeue(&test_queue,buf, 32), 32);
  rpos += 32;
  test_equal(cbfifo_length(&test_queue), cap-36);
  test_equal(strncmp(buf, str, 32), 0);
  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 12), 12);
  wpos += 12;
  test_equal(cbfifo_length(&test_queue), cap-24);

  test_equal(cbfifo_enqueue(&test_queue,str+wpos, 64), 24);  // (8)
  test_equal(cbfifo_length(&test_queue), cap);
  test_equal(cbfifo_dequeue(&test_queue,buf, cap), cap);
  test_equal(cbfifo_length(&test_queue), 0);
  test_equal(strncmp(buf, str+rpos, cap), 0);
}
void test_cbfifo()
{
  g_tests_passed = 0;
  g_tests_total = 0;

  test_cbfifo_one_iteration();
/*  if(g_tests_passed == g_tests_total)
  {
	  return 1;
  }
  else
  {*/
	  printf("Buffer Implementation : passed %d/%d test cases \n\r",
			  g_tests_passed, g_tests_total);
/*	  return 0;
  }*/
}

