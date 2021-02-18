#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>

using namespace std;

void *worker(void *arg) {
	//~ ofstream ofs("/dev/null");
	//~ devnull << "This goes to nowhere...";
    int j = 1;
    for (int i=1; i<4000000; i++) {
      // usleep(1000);
      //~ usleep(100000);
      j = (j * j) % 13331;
      j = (j * j) % 13331;
      j = (j * j) % 13331;
     
	if (i%50000==0) {
		cout << i/50000 << " being printed" << endl;
		}

      // uthread_yield();
    }
    cout << endl;
    cout << "finished worker" << endl;
    return nullptr;
}

void *longworker(void *arg) {
    int j = 1;
    for (int i=1; i<8000000; i++) {
      // usleep(1000);
      //~ usleep(100000);
      j = (j * j) % 13331;
      j = (j * j) % 13331;
      j = (j * j) % 13331;
     
      if (i%50000==0) {
		cout << i/50000 << " being printed" << endl;
		}
      // uthread_yield();
    }
    cout << endl;
    cout << "finished worker" << endl;
    return nullptr;
}

// the steps:
// setup with getcontext, makecontext, getcontext, makecontext for each thread
// start everything off with setcontext
// once one of them starts running, they call switchthreads
// switchthread calls getcontext on current thread, changes currentthread, then calls setcontext

int main(int argc, char *argv[]) {
  //
  // alarm(1);
  // sleep(1);
  int quantum_usecs = 1000;

  int ret = uthread_init(quantum_usecs);
  if (ret != 0) {
      cerr << "uthread_init FAIL!\n" << endl;
      exit(1);
  }
  //~ disableInterrupts();
  int test = 100;
  void * arg = &test;
  int tid = uthread_create(longworker, arg);

  int test2 = 100;
  void * arg2 = &test2;
  tid = uthread_create(worker, arg2);

  int test3 = 100;
  void * arg3 = &test3;
  tid = uthread_create(longworker, arg3);

  int test4 = 100;
  void * arg4 = &test4;
  tid = uthread_create(longworker, arg4);
  
  //~ usleep(100);
  //~ enableInterrupts();

  uthread_yield();
  
  //~ cout << "Part 2" << endl << endl << endl;
  //~ ////
  //~ if (ret != 0) {
      //~ cerr << "uthread_init FAIL!\n" << endl;
      //~ exit(1);
  //~ }
  //~ test = 100;
  //~ arg = &test;
  //~ tid = uthread_create(worker, arg);

  //~ test2 = 100;
  //~ arg2 = &test2;
  //~ tid = uthread_create(worker, arg2);

  //~ uthread_yield();
  
  
  
  
  return 0;

}

// Test init, yield, Create
// interrupts
