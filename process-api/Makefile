CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = p1 p2 q1 q2 q3
target = $(lastword $(binaries))

all: $(binaries)

.PHONY: clean run
clean:
	rm -r -f *.o $(binaries) *.dSYM temp

run: all
	./$(target)
