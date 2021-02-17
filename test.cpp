#include "uthread.h"
#include <unistd.h>
#include <iostream>

using namespace std;

void *worker(void *arg) {
    for (int i=0; i<4; i++) {
      sleep(1000);
      cout << i;
    }
    cout << endl;
    return nullptr;
}

int main(int argc, char *argv[]) {
  int quantum_usecs = 1000;
  int thread_count = 5;
  int thread_args = 5;
  int *threads = new int[thread_count];

  int ret = uthread_init(quantum_usecs);
  if (ret != 0) {
      cerr << "uthread_init FAIL!\n" << endl;
      exit(1);
  }

  for (int i = 0; i < thread_count; i++) {
      int tid = uthread_create(worker, &thread_args);
      threads[i] = tid;
  }

  

}

// Test init, yield, Create
// interrupts
