#ifndef _UTHREADS_H
#define _UTHREADS_H

/*
 * User-Level Threads Library (uthreads)
 * Author: OS, huji.os.2015@gmail.com
 */

#define MAX_THREAD_NUM 100 /* maximal number of threads */
#define STACK_SIZE 4096 /* stack size per thread (in bytes) */

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

#endif
