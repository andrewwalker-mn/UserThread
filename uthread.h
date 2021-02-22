

#ifndef _UTHREADS_H
#define _UTHREADS_H

class TCB;
TCB * popFromReadyQueue();


#include "TCB.h"
#include <deque>

/*
 * User-Level Threads Library (uthreads)
 * Author: OS, huji.os.2015@gmail.com
 */

#define MAX_THREAD_NUM 100 /* maximal number of threads */
#define STACK_SIZE 4096 /* stack size per thread (in bytes) */


int getsize();
/* Initialize the thread library */
// Return 0 on success, -1 on failure
// GET DONE
int uthread_init(int quantum_usecs);

/* Create a new thread whose entry point is f */
// Return new thread ID on success, -1 on failure
// GET DONE
int uthread_create(void* (*start_routine)(void*), void* arg);

/* Join a thread */
// Return 0 on success, -1 on failure
// GET DONE
int uthread_join(int tid, void **retval);

/* yield */
// Return 0 on success, -1 on failure
int uthread_yield(void);

/* Terminate this thread */
// Does not return to caller. If this is the main thread, exit the program
void uthread_exit(void *retval);

/* Suspend a thread */
// Return 0 on success, -1 on failure
int uthread_suspend(int tid);

/* Resume a thread */
// Return 0 on success, -1 on failure
int uthread_resume(int tid);

/* Get the id of the calling thread */
// Return the thread ID
int uthread_self();

/* Get the total number of library quantums (times the quantum has been set) */
// Return the total library quantum set count
int uthread_get_total_quantums();

/* Get the number of thread quantums (times the quantum has been set for this thread) */
// Return the thread quantum set count
int uthread_get_quantums(int tid);

void startInterruptTimer(); //used to not be here
void disableInterrupts(); //used to not be here
void enableInterrupts(); //used to not be here

typedef struct finished_queue_entry {
  TCB *tcb;             // Pointer to TCB
  void *result;         // Pointer to thread result (output)
} finished_queue_entry_t;

// Join queue entry type
typedef struct join_queue_entry {
  TCB *tcb;             // Pointer to TCB
  int waiting_for_tid;  // TID this thread is waiting on
} join_queue_entry_t;

//~ static deque<TCB*> ready_queue;
//~ static deque<join_queue_entry_t> block_queue;
//~ static deque<finished_queue_entry_t> finished_queue;

TCB* getThread(int tid);
void addToReadyQueue(TCB *tcb);
TCB* popFromReadyQueue();
int removeFromReadyQueue(int tid);
void addToBlockQueue(TCB *tcb, int waiting_for_tid);
join_queue_entry_t popFromBlockQueue();
int removeFromBlockQueue(int tid);
void addToFinishedQueue(TCB *tcb, void *result);
finished_queue_entry_t popFromFinishedQueue();
int removeFromFinishedQueue(int tid);


bool isFinished(int tid); //function to check if tid is in the finished queue
finished_queue_entry_t getFinished(int tid);//" to get an element of the finished queue via its tid
bool hasWaiter(int tid); //see if there is an entry in the block queue with waiter tid
join_queue_entry_t getWaiter(int tid); //return the queue entry that is waiting on tid 

#endif
