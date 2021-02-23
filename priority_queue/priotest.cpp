#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>

using namespace std;

void *worker(void *arg) {
    for (int i=0; i<4; i++) {
      showQueues();
      uthread_yield();
    }
    
    cout << endl << "finished worker" << endl;
    return nullptr;
}

int main(int argc, char *argv[]) {
  int quantum_usecs = 1000;

  int ret = uthread_init(quantum_usecs);
  if (ret != 0) {
      cerr << "uthread_init FAIL!\n" << endl;
      exit(1);
  }

  int test = 100;
  void * arg = &test;
  int tid = uthread_create(worker, arg);
  int tid2 = uthread_create(worker, arg);
  int tid3 = uthread_create(worker, arg);
  
  for (int i=0; i<4; i++) {
    getThread(tid3)->increaseQuantum();
  }

  uthread_yield();

	unsigned long *local_cnt;
	uthread_join(1,(void**)&local_cnt);
	uthread_join(2,(void**)&local_cnt);
  uthread_join(3,(void**)&local_cnt);

	cout << endl << "finished main" << endl;
  return 0;

}
