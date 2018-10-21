CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = p1 p2 q1 q2 q3 q4 q5 q6
target = $(lastword $(binaries))

all: $(binaries)

.PHONY: clean run run-q5 run-q6
clean:
	rm -r -f *.o $(binaries) *.dSYM temp

run: all
	./$(target)

# q5 and q6 are special, require some extra filtering
run-q5: q5
	./q5 | grep child -n -C 5

run-q6: q6
	./q6 | grep child -n -C 5
