#include "uthread.h"
// #include "TCB.h"
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

// not used for now, will be used in the future
static deque<join_queue_entry_t> block_queue;
static deque<finished_queue_entry_t> finish_queue;

// small helper function for error checking
int getsize() {
  return ready_queue.size();
}

// helper function just to test things out - won't be used later
deque<TCB*> getQueue() {
  return ready_queue;
}

// another helper function for testing
void get_length() {
  cout << "size" << endl;
  cout << ready_queue.size() << endl;
}

// thread ID
// is incremented every time a new thread is created, so that each thread ID is unique
static int cur_ID = 0;

// track current thread
// makes it easy to work with the current  thread and its parameters
TCB* cur_thread;

// Interrupt Management --------------------------------------------------------

// Start a countdown timer to fire an interrupt
void startInterruptTimer() //used to be static
{
	struct itimerval new_value;
	new_value.it_value.tv_sec = 0;
	new_value.it_interval.tv_sec = 0;
	new_value.it_value.tv_usec = 10; //quantum_usecs somehow?
	new_value.it_interval.tv_usec = 10; //quantum_usecs somehow?
	setitimer(ITIMER_VIRTUAL, &new_value, NULL);
  cout << "timer is ticking" << endl;
}

// Block signals from firing timer interrupt
void disableInterrupts() //used to be static
{
	sigset_t disabledInterrupts;
	sigset_t *disabledInterrupts_p;

	disabledInterrupts_p = &disabledInterrupts;
	sigemptyset(disabledInterrupts_p);
	sigaddset(disabledInterrupts_p, SIGVTALRM);
	sigprocmask(SIG_SETMASK, disabledInterrupts_p, NULL);
}

// Unblock signals to re-enable timer interrupt
void enableInterrupts() //used to be static
{
  sigset_t disabledInterrupts;
	sigset_t *disabledInterrupts_p;

	disabledInterrupts_p = &disabledInterrupts;
	sigemptyset(disabledInterrupts_p);
	sigprocmask(SIG_SETMASK, disabledInterrupts_p, NULL);
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

// void addToQueue(TCB *tcb, State state)
// {
// 	switch (state) {
// 		case READY:
// 			ready_queue.push_back(tcb);
// 			break;
// 		case BLOCK:
// 			block_queue.push_back(tcb);
// 			break;
// 		case FINISHED:
// 			finish_queue.push_back(tcb);
// 			break;
// 		default:
// 			assert(false);
// 	}
// }
//
// TCB* popFromQueue(State state) {
// 	TCB *head;
// 	switch (state) {
// 		case READY:
// 			assert(!ready_queue.empty());
// 			head = ready_queue.front();
// 			ready_queue.pop_front();
// 			return head;
// 			break;
// 		case BLOCK:
// 			assert(!block_queue.empty());
// 			head = block_queue.front();
// 			block_queue.pop_front();
// 			return head;
// 			break;
// 		case FINISHED:
// 			assert(!finish_queue.empty());
// 			head = finish_queue.front();
// 			finish_queue.pop_front();
// 			return head;
// 			break;
// 		default:
// 			assert(false);
// 	}
// }
//
TCB* getThread(int tid)
{
	 for (deque<TCB*>::iterator iter = ready_queue.begin(); iter != ready_queue.end(); ++iter)
        {
                if (tid == (*iter)->getId())
                {
                        return *iter;
                }
        }
      // for (deque<TCB*>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
      //   {
      //           if (tid == (*iter)->getId())
      //           {
      //                   return *iter;
      //           }
      //   }
      // for (deque<TCB*>::iterator iter = finish_queue.begin(); iter != finish_queue.end(); ++iter)
      //   {
      //           if (tid == (*iter)->getId())
      //           {
      //                   return *iter;
      //           }
      //   }

        // Thread not found
        return nullptr;
}

// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
    // save current thread context
    // int ret_val = cur_thread->saveContext();
    volatile int flag = 0;
    int ret_val = getcontext(&cur_thread->_context);
    if(ret_val == -1) {
      return;
    }
    if (flag == 1) {
      return;
    }

    flag = 1;
    // push current thread to queue
    if (cur_thread->getState() == RUNNING) {
      cur_thread->setState(READY);
      addToReadyQueue(cur_thread);
    }

    cout << "switching threads; queue size is " << getsize() << endl;

    // get the next thread from queue
    TCB * next = popFromReadyQueue();
    int id = next->getId();
    cout << "switched to " << id << endl;
    if(id == 0) { //I think we can/should reimplement this part with join
      cout << "skipping 0" << endl;
      cur_thread = next;
      switchThreads();
    }
    else {
      cur_thread = next;
      cur_thread->setState(RUNNING);
      setcontext(&cur_thread->_context);
      // next->loadContext();
    }
}


// Library functions -----------------------------------------------------------

// The function comments provide an (incomplete) summary of what each library
// function must do

// Starting point for thread. Calls top-level thread function
void stub(void *(*start_routine)(void *), void *arg)
{
	void* retval = start_routine(arg);
  cout << "finishing stub" << endl;
  uthread_exit(retval);
}

void sighandler(int signo) {
  switch (signo) {
    case SIGVTALRM:
      cout << "interrupt-and-yield" << endl;
      uthread_yield();
      break;
  }
}

int uthread_init(int quantum_usecs)
{
        cout << "initializing" << endl;
        TCB *new_thread = new TCB(cur_ID, nullptr, nullptr, RUNNING);
        cur_thread = new_thread;

        struct sigaction act = {0};
        act.sa_handler = sighandler;
        sigaction(SIGVTALRM, &act, NULL);

        startInterruptTimer();
        enableInterrupts();

        return 0;
}

int uthread_create(void* (*start_routine)(void*), void* arg)
{
  // Create a new thread and add it to the ready queue
  //~ disableInterrupts();
  cur_ID += 1;
  TCB *new_thread = new TCB(cur_ID, start_routine, arg, READY);
  addToReadyQueue(new_thread);

  return cur_ID;
}


// not functional yet
int uthread_join(int tid, void **retval)
{
  if(isFinished(tid)) {
    finished_queue_entry_t temp = getFinished(tid);
    *retval = temp.result;
    return 1
  }
  else {
    if(isReady(tid) || isBlocked(tid)) {
      cur_thread->setState(BLOCK);
      join_queue_entry_t temp = {cur_thread, tid};
      addToBlockQueue(temp);
      uthread_yield();
      finished_queue_entry_t temp2 = getFinished(tid);
      *retval = temp2.result;
      return 1;
    }
    else {
      return -1;
    }
  }
  // TCB *new_thread = getThread(tid);
  // if (new_thread)
  // while is still running, block
  // check to see if it's in the terminated queue. If it is, change retval accordingly and finish.
  // if it's in the running queue, add the caller thread to the join queue and block
  // once it finishes, add the caller thread to the ready queue so it can continue
  // if already terminated, continue. after it terminates, continue.
  // while(cur_thread->getState() != FINISHED) {
  //
  // }
  // cout << "finished" << endl;
  // void* ret = nullptr;// what is returned
  // if (ret != nullptr) {
  //   *retval = ret;
  // }
  return 1;
        // If the thread specified by tid is already terminated, just return
        // If the thread specified by tid is still running, block until it terminates
        // Set *retval to be the result of thread if retval != nullptr
}

int uthread_yield(void)
{
    //disableInterrupts();
    switchThreads();
    //enableInterrupts();
  // sigaction activates switch through
        // TODO
}

void uthread_exit(void *retval)
{
  int cur_id = cur_thread->getId();
  if(hasWaiter(cur_id)) {
    join_queue_entry_t temp = getWaiter(cur_id);
    TCB *blocked = temp.tcb;
    removeFromBlockQueue(blocked->getId());
    addToReadyQueue(blocked->getId());
    blocked->setState(READY);
  }
  cur_thread->setState(FINISHED);
  finished_queue_entry_t temp2 = {cur_thread, retval};
  addToFinishedQueue(temp2);
  uthread_yield();
  // check if there is a thread waiting on this one using curthread tid
  // if there's a thread waiting on this one, send it to the ready queue and yield control
  // send the current thread to the finished queue with its return values
  // yield control to the next thread in the ready queue
  // if(getsize() > 0) {
  //   TCB * temp = popFromReadyQueue();
  //   // cout << "thread exited. tid " << temp->getId() << endl;
  //
  //   if(temp->getId() == 0) {
  //     cout << "main thread done" << endl;
  //   }
  //   else {
	//      cout << "exited. tid " << temp->getId() << endl;
  //      cur_thread = temp;
  //      setcontext(&cur_thread->_context);
  //      // temp->loadContext();
  //   }
  // }
  // else {
  //   cout << "this should never happen. uthread_exit" << endl;
  //   return;
  // }
  // cur_thread->setState(FINISHED);
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
  return cur_thread->getId();
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
