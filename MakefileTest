CC = g++
CFLAGS = -lrt -g
DEPS = TCB.h uthread.h
OBJ = TCB.o uthread.o test.o
OTHER = TCB.o uthread.o main.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

uthread-demo: $(OTHER)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f uthread-demo *.o
