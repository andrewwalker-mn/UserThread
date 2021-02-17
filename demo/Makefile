CC = g++
CFLAGS = -lrt -g

demo: ucontext-demo.cpp
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f demo
