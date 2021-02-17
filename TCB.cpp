#include "TCB.h"

TCB::TCB(int tid, void *(*start_routine)(void* arg), void *arg, State state)
{
}

TCB::~TCB()
{
}

void TCB::setState(State state)
{
}

State TCB::getState() const
{
}

int TCB::getId() const
{
}

void TCB::increaseQuantum()
{
}

int TCB::getQuantum() const
{
}

int TCB::saveContext()
{
}

void TCB::loadContext()
{
}
