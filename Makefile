CC = g++
CFLAGS = -lrt -g
DEPS = TCB.h uthread.h
OBJ = TCB.o uthread.o timertest.o
OBJ2 = TCB.o uthread.o test.o
OBJ3 = TCB.o uthread.o queuetests.o
OTHER = TCB.o uthread.o main.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

timertest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

normtest: $(OBJ2)
	$(CC) -o $@ $^ $(CFLAGS)

uthread-demo: $(OTHER)
	$(CC) -o $@ $^ $(CFLAGS)
	
queuetest: $(OBJ3)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f uthread-demo timertest normtest yaya *.o
