David Ma (maxxx818) and Andrew Walker (walk0655)

# Directory Structure:
The requested deque implementation is in the main folder. Within the subfolder "priority_queue" is our extra credit implementation. Each folder is designed to be independent and all of the following instructions can be executed in both <main_folder>/ and <main_folder>/priority_queue/

# Running and Testing the Code:
A simple make command compiles everything, as does 'make alltests'. From there, specific tests are specified below.

Calling the bash script with "bash all_tests.sh" will automatically make all tests and run all tests.

You can also make individual tests by calling their names e.g. make normtest, timertest, queuetest, suspendtest, uthread-demo.

## Tests:
### ./normtest
Demonstrates functionality of uthread_init, uthread_create, uthread_yield, uthread_join and signal handling
- The body of the code is written in test.cpp
- Initializes the main thread with uthread_init, creates some worker threads with uthread_create, and yields them over time with the timer interrupt.
- The main thread waits on each worker thread using uthread_join
- Each worker simply does some arithmetic and counts up, occasionally printing values. It should just look like a large chunk of printed numbers, with multiple repeats as different threads print the same numbers.

### ./timertest
Tests timer setup, along with toggling timer interrupts.
- The main code is written in timertest.cpp
- Shows timer functionality, along with enabling and disabling timer signals.
- As explained, just continuously prints numbers. When interrupts are turned on, numbers are interrupted by alarm rings. When interrupts are turned off, numbers are printed uninterrupted.

### ./uthread-demo
The demo code provided; demonstrates functionality of uthread_init, uthread_create, uthread_yield, uthread_join, signal handling, uthread_get_quantums, and uthread_get_total_quantums.
- Main code is written in main.cpp
- Takes in three arguments as specified - <total points> <threads> [quantum_usecs].
- By default, quantum_usecs is 1. A good value for total points is 100000000, which is high enough to switch threads many times. A good value for threads is 8.
- The code initializes threads, runs them with a worker function, and yields them based on the scheduler. The main thread joins on each thread to wait for them to complete.
- At the end of main, an estimate for pi is produced, as well as the quantums each thread went through (via both uthread_get_total_quantums and uthread_get_quantums). The quantums won't be identical for each thread, but they should be approximately the same.

### ./queuetests
Tests our various functions to manipulate the readyqueue, the blockqueue, and the finishqueue.
- Each queue has respective associated functions to add something to the queue, to pop something, to retrieve something by tid, to check if a tid exists, and to remove something by tid
- The block queue also has a function to see if there's an entry waiting on a thread with a certain tid
- This is the most 'unit test'-like of the test files.

### ./suspendtest
Tests uthread_suspend and uthread_resume, since they're not tested in the demo
- Initializes three threads and messes with them while periodically printing out the states of the ready, block, and finish queues
- The first thread runs first, which suspends and then resumes the second thread. While the first thread is running, we see that the main thread is blocked because it's waiting on the first thread, and the second thread goes to block and then is moved back to ready.
- Control is naturally yielded to the third thread, which again suspends and resumes the second thread. This time, note that the first thread is in the finished queue.
- After the third thread finishes, control is yielded back to the main thread, which finishes naturally. The second thread is skipped, since the main thread never joined on it.

### ./priotest (only for /priority_queue/ extra credit implementation)
Tests the priority queue's ability to prioritize. It shows that the threads with the fewest quantums actively in running are first run from the ready queue before any other threads. 
- Initializes three threads, and artificially increases the quanta count of the third
- Runs workers that show the state of the queue and instantly yield
- Notice that tid 1 and 2 are completed before tid 3 gets any compute time (as it's already had quanta). 
