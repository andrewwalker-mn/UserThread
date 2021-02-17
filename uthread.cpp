#include "uthread.h"
#include "TCB.h"
#include <cassert>
#include <deque>

using namespace std;

// Finished queue entry type
typedef struct finished_queue_entry {
  TCB *tcb;             // Pointer to TCB
  void *result;         // Pointer to thread result (output)
} finished_queue_entry_t;

// Join queue entry type
typedef struct join_queue_entry {
  TCB *tcb;             // Pointer to TCB
  int waiting_for_tid;  // TID this thread is waiting on
} join_queue_entry_t;

// You will need to maintain structures to track the state of threads
// - uthread library functions refer to threads by their TID so you will want
//   to be able to access a TCB given a thread ID
// - Threads move between different states in their lifetime (READY, BLOCK,
//   FINISH). You will want to maintain separate "queues" (doesn't have to
//   be that data structure) to move TCBs between different thread queues.
//   Starter code for a ready queue is provided to you
// - Separate join and finished "queues" can also help when supporting joining.
//   Example join and finished queue entry types are provided above

// Queues
static deque<TCB*> ready_queue;


// Interrupt Management --------------------------------------------------------

// Start a countdown timer to fire an interrupt
static void startInterruptTimer()
{
        // TODO
}

// Block signals from firing timer interrupt
static void disableInterrupts()
{
        // TODO
}

// Unblock signals to re-enable timer interrupt
static void enableInterrupts()
{
        // TODO
}


// Queue Management ------------------------------------------------------------

// Add TCB to the back of the ready queue
void addToReadyQueue(TCB *tcb)
{
        ready_queue.push_back(tcb);
}

// Removes and returns the first TCB on the ready queue
// NOTE: Assumes at least one thread on the ready queue
TCB* popFromReadyQueue()
{
        assert(!ready_queue.empty());

        TCB *ready_queue_head = ready_queue.front();
        ready_queue.pop_front();
        return ready_queue_head;
}

// Removes the thread specified by the TID provided from the ready queue
// Returns 0 on success, and -1 on failure (thread not in ready queue)
int removeFromReadyQueue(int tid)
{
        for (deque<TCB*>::iterator iter = ready_queue.begin(); iter != ready_queue.end(); ++iter)
        {
                if (tid == (*iter)->getId())
                {
                        ready_queue.erase(iter);
                        return 0;
                }
        }

        // Thread not found
        return -1;
}

// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
        // TODO
}


// Library functions -----------------------------------------------------------

// The function comments provide an (incomplete) summary of what each library
// function must do

// Starting point for thread. Calls top-level thread function
void stub(void *(*start_routine)(void *), void *arg)
{
        // TODO
}

int uthread_init(int quantum_usecs)
{
        // Initialize any data structures
        // Setup timer interrupt and handler
        // Create a thread for the caller (main) thread
}

int uthread_create(void* (*start_routine)(void*), void* arg)
{
        // Create a new thread and add it to the ready queue
}

int uthread_join(int tid, void **retval)
{
        // If the thread specified by tid is already terminated, just return
        // If the thread specified by tid is still running, block until it terminates
        // Set *retval to be the result of thread if retval != nullptr
}

int uthread_yield(void)
{
        // TODO
}

void uthread_exit(void *retval)
{
        // If this is the main thread, exit the program
        // Move any threads joined on this thread back to the ready queue
        // Move this thread to the finished queue
}

int uthread_suspend(int tid)
{
        // Move the thread specified by tid from whatever state it is
        // in to the block queue
}

int uthread_resume(int tid)
{
        // Move the thread specified by tid back to the ready queue
}

int uthread_self()
{
        // TODO
}

int uthread_get_total_quantums()
{
        // TODO
}

int uthread_get_quantums(int tid)
{
        // TODO
}
