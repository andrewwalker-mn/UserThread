#include "TCB.h"

TCB::TCB(int tid, void *(*start_routine)(void* arg), void *arg, State state)
{
	_tid = tid;
	_state = state;
	char _stack[STACK_SIZE]; // im p sure this isnt how this works -- or should this just be the stack pointer?
	_quantum = 0; //how to get this from uthread.cpp -> quantum_usecs

	std::cout << "initializing " << _tid <<  std::endl;
	//From demo.cpp
	if (start_routine == nullptr) {
		getcontext(&_context);
		_context.uc_stack.ss_sp = new char[STACK_SIZE];
	  _context.uc_stack.ss_size = STACK_SIZE;
	  _context.uc_stack.ss_flags = 0;
	}
	else {
		getcontext(&_context);
		_context.uc_stack.ss_sp = new char[STACK_SIZE];
	  _context.uc_stack.ss_size = STACK_SIZE;
	  _context.uc_stack.ss_flags = 0;

	    makecontext(&_context, (void(*)())stub, 2, start_routine, arg);
	}
		// stub(start_routine, arg);
	/* Need to init the following
	----int _tid;---------------// The thread id number.--
	----int _quantum;           // The time interval, as explained in the pdf.--
	----State _state;-----------// The state of the thread--
	----char* _stack;-----------// The thread's stack--
    ucontext_t _context;    // The thread's saved context
	 */
}

TCB::~TCB()
{
}

void TCB::setState(State state)
{
	_state = state;
}

State TCB::getState() const
{
	return _state;
}

int TCB::getId() const
{
	return _tid;
}

void TCB::increaseQuantum()
{
	_quantum++;
}

int TCB::getQuantum() const
{
	return _quantum;
}

int TCB::saveContext()
{
	return getcontext(&_context);
}

void TCB::loadContext()
{
	setcontext(&_context);
}
