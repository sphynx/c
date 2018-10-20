CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99 -O2

all: my-base64 hex2base64

my-base64: hex.o base64.o my-base64.o
	gcc $(CFLAGS) -o $@ $^

hex2base64:	hex.o base64.o hex2base64.o
	gcc $(CFLAGS) -o $@ $^

hex.o: hex.c hex.h
	gcc -c $^

base64.o: base64.c base64.h
	gcc -c $^

my-base64.o: my-base64.c
	gcc -c $^

hex2base64.o: hex2base64.c
	gcc -c $^

.PHONY: clean test

clean:
	rm -r -f my-base64 hex2base64 *.o *.gch *.dSYM

test: my-base64
	shelltest . --color -p
	./random_test.sh
