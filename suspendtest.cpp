#include "uthread.h"
#include <iostream>

using namespace std;

void * worker(void * arg) {
  cout << "worker" << endl;
  uthread_suspend(2);
  showQueues();
  uthread_resume(2);
  showQueues();
  sleep(1);
  return nullptr;
}

int main(int argc, char *argv[]) {
  int a = 0;
  void* arg = &a;
  // worker(arg);

  uthread_init(10);

  int thread_count = 3;
  int *threads = new int[thread_count];

  for (int i = 0; i < thread_count; i++) {
      int tid = uthread_create(worker, &a);
      threads[i] = tid;
  }
  cout << "threads created" << endl;

  showQueues();

  // Wait for all threads to complete
  unsigned long *local_cnt;
  uthread_join(1, (void**)&local_cnt);

  cout << "finished" << endl;

  return 0;
}
