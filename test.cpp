#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>

using namespace std;

void wastetime(int length) {
	int j = 1;
    for (int i=1; i<length; i++) {
      j = (j * j) % 13331;
      j = (j * j) % 13331;
      j = (j * j) % 13331;

	if (i%50000==0) {
		cout << i/50000 << " " << flush;
		}
    }
}

void *worker(void *arg) {
    wastetime(4000000);

    cout << endl << "finished worker" << endl;
    return nullptr;
}

void *longworker(void *arg) {
    wastetime(8000000);

    cout << endl << "finished worker" << endl;
    return nullptr;
}

// the steps:
// setup with getcontext, makecontext, getcontext, makecontext for each thread
// start everything off with setcontext
// once one of them starts running, they call switchthreads
// switchthread calls getcontext on current thread, changes currentthread, then calls setcontext

int main(int argc, char *argv[]) {
  int quantum_usecs = 1000;

  int ret = uthread_init(quantum_usecs);
  if (ret != 0) {
      cerr << "uthread_init FAIL!\n" << endl;
      exit(1);
  }

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

  uthread_yield();

	cout << "got to bottom of main" << endl;
	cout << "total quantum: " << uthread_get_total_quantums() << endl;

  return 0;

}
