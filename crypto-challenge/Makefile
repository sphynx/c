CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

binaries = encode-base64 decode-base64 score-tester xor-encoder \
	challenge1 challenge2 challenge3 challenge4 challenge5 \
	precalculate-pop-count break-xor
target = $(lastword $(binaries))

all: $(binaries)

challenge1: base64.o hex.o
challenge2: hex.o xor.o
challenge3: hex.o score.o xor.o
challenge4: hex.o score.o xor.o
challenge5: hex.o score.o xor.o
encode-base64: base64.o io.o
decode-base64: base64.o io.o
score-tester: hex.o score.o xor.o
xor-encoder: hex.o io.o xor.o
precalculate-pop-count: hamming.o
break-xor: base64.o hamming.o hex.o io.o score.o xor.o

include depend

.PHONY: clean test depend

clean:
	$(RM) -r -f $(binaries) *.o *.gch *.dSYM

test: $(binaries)
	@shelltest test --color -p --execdir
	@test/encode_base64.test.sh
	@test/encode_decode_round.test.sh
	@./challenge1
	@./challenge2
	@./challenge3
	@./challenge4
	@./challenge5

depend: *.c
	$(CC) -MM $(CPPFLAGS) $^ > $@

run: $(target)
	./$(target)
