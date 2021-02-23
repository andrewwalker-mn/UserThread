

#ifndef _UTHREADS_H
#define _UTHREADS_H


#include "TCB.h"
#include <deque>

class TCB;
TCB * popFromReadyQueue();

/*
 * User-Level Threads Library (uthreads)
 * Author: OS, huji.os.2015@gmail.com
 */

#define MAX_THREAD_NUM 100 /* maximal number of threads */
#define STACK_SIZE 4096 /* stack size per thread (in bytes) */

// helper function to get size of readyqueue
int getsize();

/* Initialize the thread library */
// Return 0 on success, -1 on failure
int uthread_init(int quantum_usecs);

/* Create a new thread whose entry point is f */
// Return new thread ID on success, -1 on failure
int uthread_create(void* (*start_routine)(void*), void* arg);

/* Join a thread */
// Return 0 on success, -1 on failure
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

void startInterruptTimer(int quantum_usecs); //used to not be here
void disableInterrupts(); //used to not be here
void enableInterrupts(); //used to not be here

// whole bunch of helper functions to work with the various queues
// should be rather self-explanatory, since each function is quite simple

typedef struct finished_queue_entry {
  TCB *tcb;             // Pointer to TCB
  void *result;         // Pointer to thread result (output)
} finished_queue_entry_t;

// Join queue entry type
typedef struct join_queue_entry {
  TCB *tcb;             // Pointer to TCB
  int waiting_for_tid;  // TID this thread is waiting on
} join_queue_entry_t;

TCB* getThread(int tid); // get the TCB specified by tid
void addToReadyQueue(TCB *tcb); // add a TCB to the ready queue
TCB* popFromReadyQueue(); // pop the next TCB from the ready queue
int removeFromReadyQueue(int tid); // remove the TCB specified by tid from the ready queue
void addToBlockQueue(TCB *tcb, int waiting_for_tid); // add a specified block_queue_entry to the block queue
join_queue_entry_t* popFromBlockQueue(); // pop the next element from the block queue
int removeFromBlockQueue(int tid); // remove the specified block queue entry by its TCB tid
void addToFinishedQueue(TCB *tcb, void *result); // add a finished_queue_entry to the entry with its return value
finished_queue_entry_t* popFromFinishedQueue(); // pop the next element from the finished_queue
int removeFromFinishedQueue(int tid); // remove an element from the finished queue based on its TCB tid


bool isReady(int tid); // checks to see if a TCB with tid is in the ready queue
bool isBlocked(int tid); // checks to see if a TCB with tid is in the join queu
join_queue_entry_t* getBlocked(int tid); // gets the element from the join queeu with TCB tid
bool isFinished(int tid); //function to check if tid is in the finished queue
finished_queue_entry_t* getFinished(int tid);//" to get an element of the finished queue via its tid
bool hasWaiter(int tid); //see if there is an entry in the block queue with waiter tid
join_queue_entry_t* getWaiter(int tid); //return the queue entry that is waiting on tid
int getsize();
void showQueues(); // prints the tid of all elements in each queue



#endif
