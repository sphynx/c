CFLAGS = -Wall -W -Wextra -g -O2

all: run

fenwick: fenwick.o
naive: naive.o

.PHONY: clean run

run: fenwick naive
	time ./fenwick
	time ./naive

clean:
	rm *.o fenwick naive
