#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>

using namespace std;

void *worker(void *arg) {
    int j = 1;
    for (int i=1; i<10; i++) {
      // usleep(1000);
      sleep(1);
      // j = (j * i) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      // j = (j * j) % 13331;
      cout << i << " being printed" << endl;
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
  int test = 100;
  void * arg = &test;
  int tid = uthread_create(worker, arg);

  int test2 = 100;
  void * arg2 = &test2;
  tid = uthread_create(worker, arg2);

  int test3 = 100;
  void * arg3 = &test3;
  tid = uthread_create(worker, arg3);

  int test4 = 100;
  void * arg4 = &test4;
  tid = uthread_create(worker, arg4);

  uthread_yield();

  // int quantum_usecs = 1000;
  // int thread_count = 5;
  // int thread_args = 5;
  // int *threads = new int[thread_count];
  //
  // int ret = uthread_init(quantum_usecs);
  // if (ret != 0) {
  //     cerr << "uthread_init FAIL!\n" << endl;
  //     exit(1);
  // }
  //
  // for (int i = 0; i < thread_count; i++) {
  //     int tid = uthread_create(worker, &thread_args);
  //     threads[i] = tid;
  // }
  //
  // // cout << "TIME TO GO BABY "<<endl;
  //
  // uthread_yield();

  return 0;

}

// Test init, yield, Create
// interrupts
