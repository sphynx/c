# change to your open SSL installation
OPEN_SSL = /usr/local/opt/openssl-debug
CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99
LDFLAGS = -L$(OPEN_SSL)/lib
CPPFLAGS = -I$(OPEN_SSL)/include
vpath %.a $(OPEN_SSL)/lib

binaries = encode-base64 decode-base64 score-tester xor-encoder \
	challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 \
	precalculate-pop-count challenge7 challenge8 challenge9 challenge10 \
	test-padding test-aes challenge11 challenge12 challenge13 challenge14 \
	challenge15 challenge16 challenge17 challenge18 challenge19 challenge21 \
	challenge22 challenge23
target = $(lastword $(binaries))

all: $(binaries)

challenge1: base64.o hex.o
challenge2: hex.o xor.o
challenge3: hex.o score.o xor.o
challenge4: hex.o score.o xor.o
challenge5: hex.o score.o xor.o
challenge6: base64.o hamming.o hex.o io.o score.o xor.o
challenge7: -lcrypto base64.o io.o aes.o endian.o xor.o hex.o pkcs7-padding.o
challenge8: hex.o
challenge9: pkcs7-padding.o
challenge10: -lcrypto base64.o io.o aes.o endian.o xor.o hex.o pkcs7-padding.o
challenge11: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o
challenge12: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o
challenge13: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o
challenge14: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o
challenge15: pkcs7-padding.o
challenge16: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o
challenge17: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o
challenge18: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o
challenge19: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o base64.o score.o
challenge21: mersenne_twister.o
challenge22: mersenne_twister.o
challenge23: mersenne_twister.o
encode-base64: base64.o io.o
decode-base64: base64.o io.o
score-tester: hex.o score.o xor.o
xor-encoder: hex.o io.o xor.o
precalculate-pop-count: hamming.o
test-padding: pkcs7-padding.o
test-aes: -lcrypto pkcs7-padding.o aes.o endian.o xor.o hex.o random.o

include depend

.PHONY: clean test depend run check

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
	@./test-aes
	shelltest test -p -a --color --execdir

depend: *.c
	$(CC) -MM $(CPPFLAGS) $^ > $@

run: $(target)
	./$(target)

check:
	cppcheck *.c
