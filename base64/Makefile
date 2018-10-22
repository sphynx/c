CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = my-base64 hex2base64 xor

sources = *.c

all: test

my-base64: base64.o my-base64.o
hex2base64:	hex.o base64.o hex2base64.o
xor: xor.o hex.o

depend: $(sources)
	$(CC) -MM $(CPPFLAGS) $^ > $@

include depend

.PHONY: clean test

clean:
	$(RM) -r -f $(binaries) *.o *.gch *.dSYM

test: $(binaries)
	shelltest test --color -p --execdir
	test/random_test.sh
	./xor
