#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>
#include <cassert>
//~ #include "uthread.cpp"

using namespace std;

int main(int argc, char *argv[]) {
  TCB *tcb = new TCB(0, nullptr, nullptr, READY);
  
  //~ Ready queue tests
  assert(isReady(0) == false);
  addToReadyQueue(tcb);
  assert(isReady(0));
  assert(popFromReadyQueue() == tcb);
  addToReadyQueue(tcb);
  assert(removeFromReadyQueue(0) == 0);
  assert(removeFromReadyQueue(0) == -1);

  //~ Block queue tests
  assert(isBlocked(0) == false);
  assert(hasWaiter(1) == false);
  addToBlockQueue(tcb, 1);
  assert(isBlocked(0));
  assert(hasWaiter(1));
  
  join_queue_entry_t* entry1 = getWaiter(1);
  assert(entry1->tcb == tcb);
  assert(entry1->waiting_for_tid == 1);
  entry1 = popFromBlockQueue();
  assert(entry1->tcb == tcb);
  assert(entry1->waiting_for_tid == 1);
  addToBlockQueue(tcb, 1);
  assert(removeFromBlockQueue(0) == 0);
  assert(removeFromBlockQueue(0) == -1);
  
  //~ Finished queue tests
  void *result = nullptr;
  assert(isFinished(0) == false);
  addToFinishedQueue(tcb, result);
  assert(isFinished(0));
  finished_queue_entry_t* entry2 = getFinished(0);
  assert(entry2->tcb == tcb);
  assert(entry2->result == nullptr);

  entry2 = popFromFinishedQueue();
  assert(entry2->tcb == tcb);
  assert(entry2->result == nullptr);

  
}
