CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = my-base64 score-tester challenge1 challenge2 challenge3 challenge4 challenge5
target = $(lastword $(binaries))

all: $(binaries)

my-base64: base64.o my-base64.o
score-tester: score-tester.o hex.o xor.o score.o
challenge1: challenge1.o base64.o hex.o
challenge2: challenge2.o xor.o hex.o
challenge3: challenge3.o xor.o hex.o score.o
challenge4: challenge4.o hex.o xor.o score.o
challenge5: challenge5.o hex.o xor.o score.o

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
