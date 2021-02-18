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
static deque<TCB*> block_queue;
static deque<TCB*> finish_queue;

int getsize() {
  return ready_queue.size();
}

deque<TCB*> getQueue() {
  return ready_queue;
}


void get_length() {
  cout << "size" << endl;
  cout << ready_queue.size() << endl;
  // cout << "asdfsdfsdfsd" << endl;
}

// thread ID
static int cur_ID = 0;

// track current thread

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

// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
    int ret_val = cur_thread->saveContext();
    if(ret_val == -1) {
      return;
    }
    addToReadyQueue(cur_thread);
    cout << "switching threads and size is " << getsize() << endl;

    // cout << "state of deque " << endl;
    // for (int i = 0; i < ready_queue.size(); i++)
    //     std::cout << ready_queue[i]->getId() <<  " ";
    // std::cout << '\n';

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
      next->loadContext();
    }
    // cur_thread = next;
    // next->loadContext();

    // cout << "current id " << cur_thread->getId();

        // TODO
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
      cout << "fuck yourself" << endl;
      // startInterruptTimer();
      uthread_yield();
      // exit(3);
      break;
  }
}

int uthread_init(int quantum_usecs)
{
        cout << " something changed " << endl;
        TCB *new_thread = new TCB(cur_ID, nullptr, nullptr, READY);
        cur_thread = new_thread;
        // addToReadyQueue(new_thread);
        // enableInterrupts();
        // startInterruptTimer();

        struct sigaction act = {0};

        act.sa_handler = sighandler;
        sigaction(SIGVTALRM, &act, NULL);
        // alarm(1);
        startInterruptTimer();
        		enableInterrupts();

        //~ disableInterrupts();
        
        //~ int j = 1;
        //~ for (int i=1; i<1000000; i++) {
      //~ // usleep(1000);
      //~ usleep(100000);
      //~ j = (j * j) % 13331;
      //~ j = (j * j) % 13331;
      //~ j = (j * j) % 13331;
     
	//~ if (i%500==0) {
		//~ cout << "init" << i/500 << " being printed" << endl;
		//~ }

      //~ // uthread_yield();
    //~ }
    
        // Initialize any data structures
        // Setup timer interrupt and handler
        // Create a thread for the caller (main) thread
        return 0;
}

int uthread_create(void* (*start_routine)(void*), void* arg)
{
  // Create a new thread and add it to the ready queue
  //~ disableInterrupts();
  cur_ID += 1;
  TCB *new_thread = new TCB(cur_ID, start_routine, arg, READY);
  addToReadyQueue(new_thread);
  //~ enableInterrupts();
  // new_thread->loadContext();
  
  return cur_ID;
}

int uthread_join(int tid, void **retval)
{

  while(cur_thread->getState() != FINISHED) {
    
  }
  cout << "finished" << endl;
  retval = (void**) 1;
  uthread_yield();


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
  if(getsize() > 0) {
    TCB * temp = popFromReadyQueue();
    // cout << "state of deque " << endl;
    // for (int i = 0; i < ready_queue.size(); i++)
    //     std::cout << ready_queue[i]->getId() <<  " ";
    // std::cout << '\n';
    	  cout << "exited. tid " << temp->getId() << endl;

    if(temp->getId() == 0) {
      cout << "we done" << endl;
    }
    else {
	//~ if (temp->getId() == 1) {
		//~ disableInterrupts();
		//~ cout << "11111111111111disabled1111111111111111" << endl;
	//~ }
    //~ if (temp->getId() > -1) {
		
		//~ cout << "1111111111111enabled11111111111111111" << endl;

	//~ }
	  cout << "exited. tid " << temp->getId() << endl;
      cur_thread = temp;
      temp->loadContext();
    }
  }
  else {
    cout << "fuck off" << endl;
    return;
  }
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
