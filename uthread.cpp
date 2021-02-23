#include "uthread.h"
// #include "TCB.h"
#include <cassert>
#include <deque>


using namespace std;

// Finished queue entry type
// typedef struct finished_queue_entry {
//   TCB *tcb;             // Pointer to TCB
//   void *result;         // Pointer to thread result (output)
// } finished_queue_entry_t;
//
// // Join queue entry type
// typedef struct join_queue_entry {
//   TCB *tcb;             // Pointer to TCB
//   int waiting_for_tid;  // TID this thread is waiting on
// } join_queue_entry_t;

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
static deque<join_queue_entry_t> block_queue;
static deque<finished_queue_entry_t> finished_queue;

// this one tracks every thread, to make certain operations easier
static deque<TCB*> everything;

// small helper function for error checking
int getsize() {
  return ready_queue.size();
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
// makes it easy to work with the current thread and its elements
TCB* cur_thread;

// simply trakcs the number of user seconds per quantum for later use; initialized in init
int num_usecs;

// Interrupt Management --------------------------------------------------------

// Start a countdown timer to fire an interrupt
void startInterruptTimer(int quantum_usecs) //used to be static
{
	struct itimerval new_value;
	new_value.it_value.tv_sec = 0;
	new_value.it_interval.tv_sec = 0;
	new_value.it_value.tv_usec = quantum_usecs; //quantum_usecs somehow?
	new_value.it_interval.tv_usec = quantum_usecs; //quantum_usecs somehow?
	setitimer(ITIMER_VIRTUAL, &new_value, NULL);
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

void addToBlockQueue(TCB *tcb, int waiting_for_tid)
{
	join_queue_entry entry{};
	entry.tcb = tcb;
	entry.waiting_for_tid = waiting_for_tid;
	block_queue.push_back(entry);
}

join_queue_entry_t* popFromBlockQueue()
{
	assert(!block_queue.empty());

	join_queue_entry_t* block_queue_head = &block_queue.front();
	block_queue.pop_front();
	return block_queue_head;
}


int removeFromBlockQueue(int tid)
{
	for (deque<join_queue_entry_t>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        block_queue.erase(iter);
                        return 0;
                }
        }

        // Thread not found
        return -1;
}

void addToFinishedQueue(TCB *tcb, void *result)
{
	finished_queue_entry_t entry{};
	entry.tcb = tcb;
	entry.result = result;
	finished_queue.push_back(entry);
}

finished_queue_entry_t* popFromFinishedQueue()
{
	assert(!finished_queue.empty());

	finished_queue_entry_t* finished_queue_head = &finished_queue.front();
	finished_queue.pop_front();
	return finished_queue_head;
}

int removeFromFinishedQueue(int tid)
{
	for (deque<finished_queue_entry_t>::iterator iter = finished_queue.begin(); iter != finished_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        finished_queue.erase(iter);
                        return 0;
                }
        }

        // Thread not found
        return -1;
}

TCB* getThread(int tid)
{
	 for (deque<TCB*>::iterator iter = ready_queue.begin(); iter != ready_queue.end(); ++iter)
        {
                if (tid == (*iter)->getId())
                {
                        return *iter;
                }
        }
        // Thread not found
        return nullptr;
}

bool isReady(int tid)
{
	for (deque<TCB*>::iterator iter = ready_queue.begin(); iter != ready_queue.end(); ++iter)
        {
                if (tid == (*iter)->getId())
                {
                        return true;
                }
        }
	return false;
}

bool isBlocked(int tid) {
	for (deque<join_queue_entry_t>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        return true;
                }
        }
        return false;
}

join_queue_entry_t* getBlocked(int tid) {
	for (deque<join_queue_entry_t>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        return &(*iter);
                }
        }
        return nullptr;
}


bool isFinished(int tid)
{
	for (deque<finished_queue_entry_t>::iterator iter = finished_queue.begin(); iter != finished_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        return true;
                }
        }
        return false;
}

finished_queue_entry_t* getFinished(int tid)
{
	for (deque<finished_queue_entry_t>::iterator iter = finished_queue.begin(); iter != finished_queue.end(); ++iter)
        {
                if (tid == (*iter).tcb->getId())
                {
                        return &(*iter);
                }
        }

        // Thread not found
        assert(false);
}

bool hasWaiter(int tid)
{
	for (deque<join_queue_entry_t>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
        {
                if (tid == (*iter).waiting_for_tid)
                {
                        return true;
                }
        }

        return false;
}

join_queue_entry_t* getWaiter(int tid)
{
	for (deque<join_queue_entry_t>::iterator iter = block_queue.begin(); iter != block_queue.end(); ++iter)
        {
                if (tid == (*iter).waiting_for_tid)
                {
                        return &(*iter);
                }
        }
        assert(false);
}

void showQueues() {
  cout << "state of ready queue" << endl;
  for (int i = 0; i < ready_queue.size(); i++)
      std::cout << ready_queue[i]->getId() <<  " ";
  std::cout << '\n';

  cout << "state of block queue" << endl;
  for (int i = 0; i < block_queue.size(); i++)
      std::cout << block_queue[i].tcb->getId() <<  " ";
  std::cout << '\n';

  cout << "state of finish queue" << endl;
  for (int i = 0; i < finished_queue.size(); i++)
      std::cout << finished_queue[i].tcb->getId() <<  " ";
  std::cout << '\n';
}

// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
    disableInterrupts();
    // save current thread context
    volatile int flag = 0;
    int ret_val = getcontext(&cur_thread->_context);
    if(ret_val == -1) {
      enableInterrupts();
      return;
    }
    if (flag == 1) {
      enableInterrupts();
      return;
    }

    flag = 1;
    // push current thread to the back of the queue
    // checks if the current thread is running because this can be called after threads block as well
    if (cur_thread->getState() == RUNNING) {
      cur_thread->setState(READY);
      addToReadyQueue(cur_thread);
    }

    // get the next thread from queue and load its context
    TCB * next = popFromReadyQueue();
    int id = next->getId();
    cur_thread = next;
    cur_thread->setState(RUNNING);
    setcontext(&cur_thread->_context);
    enableInterrupts();
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
      cur_thread->increaseQuantum();
      uthread_yield();
      break;
  }
}

int uthread_init(int quantum_usecs)
{
        cout << "initializing" << endl;
        // the TCB constructor recognizes the nullptr, and thus doesn't make a new context
        num_usecs = quantum_usecs;
        TCB *new_thread = new TCB(cur_ID, nullptr, nullptr, RUNNING);
        cur_thread = new_thread;
        everything.push_back(new_thread);

        struct sigaction act = {0};
        act.sa_handler = sighandler;
        sigaction(SIGVTALRM, &act, NULL);

        startInterruptTimer(quantum_usecs);
        enableInterrupts();

        return 0;
}

int uthread_create(void* (*start_routine)(void*), void* arg)
{
  // Create a new thread and add it to the ready queue
 if (cur_ID == MAX_THREAD_NUM) {
    throw runtime_error("Max thread number exceeded; please try with fewer threads");
  }

  cur_ID += 1;
  TCB *new_thread = new TCB(cur_ID, start_routine, arg, READY);
  disableInterrupts();
  addToReadyQueue(new_thread);
  everything.push_back(new_thread);
  enableInterrupts();

  return cur_ID;
}

int uthread_join(int tid, void **retval)
{
    disableInterrupts();
    // if the specified thread has already terminated, just change retval accordingly.
    if(isFinished(tid)) {
    finished_queue_entry_t* temp = getFinished(tid);
    *retval = temp->result;
    delete temp->tcb;
    removeFromFinishedQueue(tid);
    enableInterrupts();
    return 1;
  }
    else {
      // if the specified thread is ready or is blocked, move the caller thread to the blocked queue
      // then yield control. When the caller thread is given control back - because the specified thread has terminated -
      // get the return value and change retval accordingly
      if(isReady(tid) || isBlocked(tid)) {
        cur_thread->setState(BLOCK);
        addToBlockQueue(cur_thread, tid);
        uthread_yield();
        finished_queue_entry_t* temp2 = getFinished(tid);
        *retval = temp2->result;
        delete temp2->tcb;
	removeFromFinishedQueue(tid);
	enableInterrupts();
        return 0;
      }
      // if the specified thread doesn't exist, error out
      else {
        enableInterrupts();
        return -1;
      }
    }
    enableInterrupts();
    return 1;
}

int uthread_yield(void)
{
    disableInterrupts();
    switchThreads();
    enableInterrupts();
    return 0;
}

void uthread_exit(void *retval)
{
  disableInterrupts();
  int cur_id = cur_thread->getId();

  // if there is a blocked thread waiting on this thread, unblock that thread
  if(hasWaiter(cur_id)) {
    join_queue_entry_t* temp = getWaiter(cur_id);
    TCB *blocked = temp->tcb;
    removeFromBlockQueue(blocked->getId());
    addToReadyQueue(blocked);
    blocked->setState(READY);
  }

  cur_thread->setState(FINISHED);
  addToFinishedQueue(cur_thread, retval);

  // check to see if this is the main thread or not by seeing if any other threads are ready
  if(getsize() > 0) {
    uthread_yield();
  }
  else {
    cout << "main thread reached, exiting" << endl;
    exit(1);
  }
  enableInterrupts();
}

int uthread_suspend(int tid)
{
  disableInterrupts();
  // if the currently running thread is the specified tid, add it to the block queue and trigger a reschedule
  if(cur_thread->getId() == tid) {
    cur_thread->setState(BLOCK);
    addToBlockQueue(cur_thread, -1);
    startInterruptTimer(num_usecs);
    // trigger reschedule
    // reset time slice
    uthread_yield();
  }
  // otherwise, just move the specified thread from ready to blocked
  else if(isReady(tid)) {
    TCB * thread = getThread(tid);
    removeFromReadyQueue(tid);
    thread->setState(BLOCK);
    addToBlockQueue(thread, -1);
  }
  enableInterrupts();
  return 0;
}

int uthread_resume(int tid)
{
  disableInterrupts();
  // if the specified thread is in the blocked queue, move it back to ready
  if(isBlocked(tid)) {
    join_queue_entry_t* blocked = getBlocked(tid);
    removeFromBlockQueue(tid);
    addToReadyQueue(blocked->tcb);
    blocked->tcb->setState(READY);
  }
  enableInterrupts();
  return 1;
}

int uthread_self()
{
  return cur_thread->getId();
}

int uthread_get_total_quantums()
{
  // self-explanatory, and the main reason the everything queue exists
  disableInterrupts();
  int tot = 0;
  for (int i = 0; i < everything.size(); i++)
      tot += everything[i]->getQuantum();
  enableInterrupts();
  return tot;
}

int uthread_get_quantums(int tid)
{
  // also self-explanatory
  disableInterrupts();
  for (int i = 0; i < everything.size(); i++){
    if (everything[i]->getId() == tid) {
      enableInterrupts();
      return everything[i]->getQuantum();
    }
  }
  enableInterrupts();
  return -1;
}
