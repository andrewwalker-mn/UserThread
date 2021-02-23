#include "TCB.h"

TCB::TCB(int tid, void *(*start_routine)(void* arg), void *arg, State state)
{
	_tid = tid;
	_state = state;
	_quantum = 0; 
	std::cout << "initializing " << _tid <<  std::endl;

	// check to see if this is being called by main thread
	if (start_routine == nullptr) {
		getcontext(&_context);
		_context.uc_stack.ss_sp = new char[STACK_SIZE];
	  _context.uc_stack.ss_size = STACK_SIZE;
	  _context.uc_stack.ss_flags = 0;
	}
	// all other threads need a makecontext
	else {
		getcontext(&_context);
		_context.uc_stack.ss_sp = new char[STACK_SIZE];
	  _context.uc_stack.ss_size = STACK_SIZE;
	  _context.uc_stack.ss_flags = 0;

	  makecontext(&_context, (void(*)())stub, 2, start_routine, arg);
	}
}

TCB::~TCB()
{
  // nothing should be explicitly deleted in the destructor here
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
