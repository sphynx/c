CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = my-base64 hex2base64 xor-test xor-cypher

all: $(binaries)

my-base64: base64.o my-base64.o
hex2base64:	hex.o base64.o hex2base64.o
xor-test: xor.o hex.o
xor-cypher: xor-cypher.o hex.o xor.o

include depend

.PHONY: clean test depend

clean:
	$(RM) -r -f $(binaries) *.o *.gch *.dSYM

test: $(binaries)
	@shelltest test --color -p --execdir
	@test/random_test.sh
	@./xor-test

depend: *.c
	$(CC) -MM $(CPPFLAGS) $^ > $@

run: xor-cypher
	./xor-cypher
