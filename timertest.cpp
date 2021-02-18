#include "uthread.h"
#include <unistd.h>
#include "TCB.h"
#include <iostream>

using namespace std;

void sigvtalrmhandler(int signo) {
  switch (signo) {
    case SIGVTALRM:
      cout << "alarm ring" << endl;
      break;
  }
}

int main(int argc, char *argv[]) {
  
	struct sigaction act = {0};

	act.sa_handler = sigvtalrmhandler;
	sigaction(SIGVTALRM, &act, NULL);
	
	startInterruptTimer();
	
	cout << "Should print numbers 1-10 interspersed with alarm rings, then disable alarm through 60 when it turns on again" << endl;
  
	int j=1;
	for (int i=1; i<8000000; i++) {

      j = (j * j) % 13331;
      j = (j * j) % 13331;
      j = (j * j) % 13331;
    
	if (i%100000==0) {
		cout << i/100000 << " ";
		}
		
	if (i==1000000) {
		disableInterrupts();
	}
	
	if (i==6000000) {
		enableInterrupts();
	}

      // uthread_yield();
    }
  
  
	return 0;

}
