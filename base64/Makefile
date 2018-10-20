CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99
sources = *.c

all: depend my-base64 hex2base64

my-base64: base64.o my-base64.o

hex2base64:	hex.o base64.o hex2base64.o

.PHONY: clean test

clean:
	$(RM) -r -f my-base64 hex2base64 *.o *.gch *.dSYM

test: my-base64
	shelltest test --color -p --execdir
	test/random_test.sh

depend: $(sources)
	$(CC) -MM $(CPPFLAGS) $^ > $@

include depend
