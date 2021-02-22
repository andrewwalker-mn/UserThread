#include "uthread.h"
#include <iostream>

using namespace std;

void * worker(void * arg) {
  cout << "worker" << endl;
  uthread_suspend(2);
  uthread_suspend(3);
  uthread_resume(2);
  // uthread_resume(3);
  sleep(1);
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

  // Wait for all threads to complete
  //     unsigned long *local_cnt;
  //     uthread_join(threads[i], (void**)&local_cnt);
  for (int i = 0; i < thread_count; i++) {
      // Add thread result to global total
      unsigned long *local_cnt;
      uthread_join(threads[i], (void**)&local_cnt);
  }

  cout << "finished" << endl;

  return 0;
}
