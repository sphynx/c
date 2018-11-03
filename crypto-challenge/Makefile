# change to your open SSL installation
OPEN_SSL = /usr/local/opt/openssl-debug
CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99
LDFLAGS = -L$(OPEN_SSL)/lib
CPPFLAGS = -I$(OPEN_SSL)/include
vpath %.a $(OPEN_SSL)/lib

binaries = encode-base64 decode-base64 score-tester xor-encoder \
	challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 \
	precalculate-pop-count challenge7 challenge8 challenge9 challenge10 \
	test-padding
target = $(lastword $(binaries))

all: $(binaries)

challenge1: base64.o hex.o
challenge2: hex.o xor.o
challenge3: hex.o score.o xor.o
challenge4: hex.o score.o xor.o
challenge5: hex.o score.o xor.o
challenge6: base64.o hamming.o hex.o io.o score.o xor.o
challenge7: -lcrypto base64.o io.o aes.o xor.o hex.o pkcs7-padding.o
challenge8: hex.o
challenge9: pkcs7-padding.o
challenge10: -lcrypto base64.o io.o aes.o xor.o hex.o pkcs7-padding.o
encode-base64: base64.o io.o
decode-base64: base64.o io.o
score-tester: hex.o score.o xor.o
xor-encoder: hex.o io.o xor.o
precalculate-pop-count: hamming.o
test-padding: pkcs7-padding.o

# include depend

.PHONY: clean test depend

clean:
	$(RM) -r -f $(binaries) *.o *.gch *.dSYM

test: $(binaries)
	@test/encode_base64.test.sh
	@test/encode_decode_round.test.sh
	@./challenge1
	@./challenge2
	@./challenge3
	@./challenge4
	@./challenge5
	@./test-padding
	shelltest test -p -a --color --execdir

depend: *.c
	$(CC) -MM $(CPPFLAGS) $^ > $@

run: $(target)
	./$(target)
