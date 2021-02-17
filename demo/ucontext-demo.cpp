/*
 * Original code from the Multithread demo program.
 * Hebrew University OS course.
 *
 * Questions: os@cs.huji.ac.il
 *
 * Modified from original to simplify details
 */

#include <iostream>
#include <ucontext.h>
#include <unistd.h>

using namespace std;

#define SECOND 1000000
#define STACK_SIZE 4096

static ucontext_t cont[2];
static int f_params[3] = { 1, 2, 3 };

void switchThreads();

void f(int x, int y, int z)
{
    cout << "x = " << x << endl;
    cout << "y = " << y << endl;
    cout << "z = " << z << endl;

    int i = 0;
    while (1) {
        ++i;
        cout << "in f (" << i << ")" << endl;
        if (i % 3 == 0) {
            cout << "f: switching" << endl;
            switchThreads();
        }
        usleep(SECOND);
    }
}

void g()
{
    int i = 0;
    while (1) {
        ++i;
        cout << "in g (" << i << ")" << endl;
        if (i % 5 == 0) {
            cout << "g: switching" << endl;
            switchThreads();
        }
        usleep(SECOND);
    }
}


void setup()
{
    // Thread 1 ----------------------------------------------------------------

    // Get the current execution context
    getcontext(&cont[0]);

    // Modify the context to a new stack
    cont[0].uc_stack.ss_sp = new char[STACK_SIZE];
    cont[0].uc_stack.ss_size = STACK_SIZE;
    cont[0].uc_stack.ss_flags = 0;

    // Set up the context to run f as the top-level thread function
    makecontext(&cont[0], (void(*)())f, 3, f_params[0], f_params[1], f_params[2]);

    // Thread 2 ----------------------------------------------------------------

    // Get the current execution context
    getcontext(&cont[1]);

    // Modify the context to a new stack
    cont[1].uc_stack.ss_sp = new char[STACK_SIZE];
    cont[1].uc_stack.ss_size = STACK_SIZE;
    cont[1].uc_stack.ss_flags = 0;

    // Set up the context to run g as the top-level thread function
    makecontext(&cont[1], g, 0);
}


void switchThreads()
{
    // currentThread is a global variable shared by both threads
    static int currentThread = 0;

    // flag is a local stack variable to each thread
    volatile int flag = 0;

    // getcontext() will "return twice" - Need to differentiate between the two
    int ret_val = getcontext(&cont[currentThread]);
    cout << "SWITCH: currentThread = " << currentThread << endl;

    // If flag == 1 then it was already set below so this is the second return
    // from getcontext (run this thread)
    if (flag == 1) {
        return;
    }

    // This is the first return from getcontext (switching threads)
    flag = 1;
    currentThread = 1 - currentThread;
    setcontext(&cont[currentThread]);
}


void cleanup() {
    delete[] (char*)cont[0].uc_stack.ss_sp;
    delete[] (char*)cont[1].uc_stack.ss_sp;
}


int main()
{
    setup();		
    setcontext(&cont[0]);
    cleanup(); // Threads run forever so this technically won't run
    return 0;
}


/* Example output:
x = 1
y = 2
z = 3
in f (1)
in f (2)
in f (3)
f: switching
SWITCH: currentThread=0
in g (1)
in g (2)
in g (3)
in g (4)
in g (5)
g: switching
SWITCH: currentThread=1
SWITCH: currentThread=0
in f (4)
in f (5)
in f (6)
f: switching
SWITCH: currentThread=0
SWITCH: currentThread=1
in g (6)
in g (7)
in g (8)
in g (9)
in g (10)
g: switching
... runs forever */
