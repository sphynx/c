CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

all: my-base64 hex2base64

my-base64: hex.o base64.o my-base64.o
hex2base64:	hex.o base64.o hex2base64.o
hex.o: hex.c  hex.h
base64.o: base64.c base64.h
my-base64.o: my-base64.c base64.h
hex2base64.o: hex2base64.c hex.h base64.h

.PHONY: clean test

clean:
	rm -r -f my-base64 hex2base64 *.o *.gch *.dSYM

test: my-base64
	shelltest test --color -p --execdir
	test/random_test.sh
