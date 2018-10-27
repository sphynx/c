CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = my-base64 decode-base64 score-tester challenge1 challenge2 challenge3 \
	challenge4 challenge5 xor-encoder
target = $(lastword $(binaries))

all: $(binaries)

my-base64: base64.o io.o
decode-base64: base64.o io.o
score-tester: hex.o score.o xor.o
xor-encoder: hex.o io.o xor.o
challenge1: base64.o hex.o
challenge2: hex.o xor.o
challenge3: hex.o score.o xor.o
challenge4: hex.o score.o xor.o
challenge5: hex.o score.o xor.o

include depend

.PHONY: clean test depend

clean:
	$(RM) -r -f $(binaries) *.o *.gch *.dSYM

test: $(binaries)
	@shelltest test --color -p --execdir
	@test/random_test.sh
	@./challenge1
	@./challenge2
	@./challenge3
	@./challenge4
	@./challenge5

depend: *.c
	$(CC) -MM $(CPPFLAGS) $^ > $@

run: $(target)
	./$(target)
