/*
 * test01x.c
 * Test Program T1x - Multiple Thread Creation
 * Chris Meyer, CISC
 * This shows the creation of many threads, generated by both main
 * and by secondary threads, testing robustness of t_yield and TCB
 * queues, and showing different termination times of different threads.
 *
 */

#include <stdio.h>
#include "ud_thread.h"

// single thread: doesn't generate any other threads
void single_thread(int thr_id) {

   int i, j;

   for (i = j = 0; i < 10; i++, j++) {

      printf("this is thread #%d [%d]...\n", thr_id, j);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

// "double" thread: generates two single threads, keeps running
void double_thread(int thr_id) {

   // make two single threads
   int k;
   for (k = thr_id+1; k < thr_id+3; k++)
   {
      t_create(single_thread, k, 1);
      printf("this is double thread #%d, created single thread #%d.\n", thr_id, k);

   }
   t_yield();
   int i, j;

   for (i = j = 0; i < 8; i++, j++) {

      printf("this is 'double' thread %d [%d]...\n", thr_id, j);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

// "triple" thread: generates three "double" threads (each of which generates
// two single threads), keeps running
void triple_thread(int thr_id) {
   // make three double threads
   // these three new threads will be set to high priority
   int k;
   for (k = thr_id+1; k < thr_id+8; k+=3)
   {
     t_create(double_thread, k, 1);
     printf("this is triple thread #%d, created double thread #%d.\n", thr_id, k);
   }
   t_yield();
   int i, j;

   for (i = j = 0; i < 6; i++, j++) {

      printf("this is 'triple' thread %d [%d]...\n", thr_id, j);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

// "quad" thread: generates four single threads, keeps running (for shortest time)
void quad_thread(int thr_id) {
   // make four single threads
   int k;
   for (k = thr_id+1; k < thr_id+5; k++)
   {

      t_create(single_thread, k, 1);
      printf("this is quad thread #%d, created single thread #%d.\n", thr_id, k);

   }
   t_yield();
   int i, j;

   for (i = j = 0; i < 4; i++, j++) {

      printf("this is 'quad' thread #%d [%d]...\n", thr_id, j);
      t_yield();
   }

   printf("Thread %d is done...\n", thr_id);
   t_terminate();
}

int main(void){

   int i;

   // thread library, main thread starts
   t_init();
   // start with creating single thread #1
   t_create(single_thread, 1, 1);
   printf("This is main()#0 created single thread[1]...\n");
   // now, create double thread #2 (will create two single threads #3 and #4)
   t_create(double_thread, 2, 1);
   printf("This is main()#0 created double thread[2]...\n");
   // now, create triple thread #5 (will create nine more threads #6 to #14)
   t_create(triple_thread, 5, 1);
   printf("This is main()#0 created triple thread[5]...\n");
   // finally, create quad thread #15 (will create four more threads #16 to #19)
   t_create(quad_thread, 15, 1);
   printf("This is main()#0 created quad thread[15]...\n");

   // go longer than all other threads (to show when t_yield is called with empty ready queue)
   for (i = 0; i < 16; i++) {

      printf("This is main()[%d]...\n", i);
      t_yield();
   }

   // all done
   printf("Begin shutdown...\n");
   t_shutdown();
   printf("Done with shutdown...\n");

   return 0;
}
