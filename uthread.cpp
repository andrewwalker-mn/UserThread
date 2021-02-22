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


//~ // not used for now, will be used in the future
static deque<join_queue_entry_t> block_queue;
static deque<finished_queue_entry_t> finished_queue;

static deque<TCB*> everything;


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

    // cout << "switching threads; queue size is " << getsize() << endl;

    // get the next thread from queue
    TCB * next = popFromReadyQueue();
    int id = next->getId();
    cout << "switched to " << id << endl;
    // if(id == 0) { //I think we can/should reimplement this part with join
    //   cout << "skipping 0" << endl;
    //   cur_thread = next;
    //   switchThreads();
    // }
    // else {
      cur_thread = next;
      cur_thread->setState(RUNNING);
      setcontext(&cur_thread->_context);
      // next->loadContext();
    // }
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
      cur_thread->increaseQuantum();
      uthread_yield();
      break;
  }
}

int uthread_init(int quantum_usecs)
{
        cout << "initializing" << endl;
        TCB *new_thread = new TCB(cur_ID, nullptr, nullptr, RUNNING);
        cur_thread = new_thread;
        everything.push_back(new_thread);

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
  everything.push_back(new_thread);

  return cur_ID;
}


// not functional yet
int uthread_join(int tid, void **retval)
{
  if(isFinished(tid)) {
    finished_queue_entry_t* temp = getFinished(tid);
    *retval = temp->result;
    return 1;
  }
  else {
    if(isReady(tid) || isBlocked(tid)) {
      cur_thread->setState(BLOCK);
      addToBlockQueue(cur_thread, tid);
      uthread_yield();
      finished_queue_entry_t* temp2 = getFinished(tid);
      *retval = temp2->result;
      return 1;
    }
    else {
      return -1;
    }
  }
  return 1;
        // If the thread specified by tid is already terminated, just return
        // If the thread specified by tid is still running, block until it terminates
        // Set *retval to be the result of thread if retval != nullptr
}

int uthread_yield(void)
{
    switchThreads();
    return 1;
}

void uthread_exit(void *retval)
{
  int cur_id = cur_thread->getId();
  if(hasWaiter(cur_id)) {
    join_queue_entry_t* temp = getWaiter(cur_id);
    TCB *blocked = temp->tcb;
    removeFromBlockQueue(blocked->getId());
    addToReadyQueue(blocked);
    blocked->setState(READY);
  }
  cur_thread->setState(FINISHED);
  // finished_queue_entry_t temp2 = {cur_thread, retval};
  finished_queue_entry_t temp2 = {cur_thread, retval};

  addToFinishedQueue(cur_thread, retval);

  if(getsize() > 0) {
    uthread_yield();
  }
  else {
    cout << "we done for now!" << endl;
  }
}

int uthread_suspend(int tid)
{
  if(cur_thread->getId() == tid) {
    cur_thread->setState(BLOCK);
    addToBlockQueue(cur_thread, -1);
    // trigger reschedule
    // reset time slice
    uthread_yield();
  }
  else if(isReady(tid)) {
    TCB * thread = getThread(tid);
    removeFromReadyQueue(tid);
    thread->setState(BLOCK);
    addToBlockQueue(thread, -1);
  }
  return 1;
  // TCB * thread =
        // Move the thread specified by tid from whatever state it is
        // in to the block queue
}

int uthread_resume(int tid)
{
  if(isBlocked(tid)) {
    join_queue_entry_t* blocked = getBlocked(tid);
    removeFromBlockQueue(tid);
    addToReadyQueue(blocked->tcb);
    blocked->tcb->setState(READY);
  }
  return 1;
        // Move the thread specified by tid back to the ready queue
}

int uthread_self()
{
  return cur_thread->getId();
        // TODO
}

int uthread_get_total_quantums()
{
  int tot = 0;
  for (int i = 0; i < everything.size(); i++)
      tot += everything[i]->getQuantum();

  return tot;
        // TODO
}

int uthread_get_quantums(int tid)
{
  for (int i = 0; i < everything.size(); i++){
    if (everything[i]->getId() == tid) {
      return everything[i]->getQuantum();
    }
  }
  return -1;
        // TODO
}
