CFLAGS = -g -Wall -Wextra -Werror --pedantic --std=c99 -O2

all: my-base64 hex2base64

my-base64: hex.c base64.c my-base64.c
	gcc $(CFLAGS) -o my-base64 hex.c base64.c my-base64.c

hex2base64:	hex.c base64.c hex2base64.c
	gcc $(CFLAGS) -o hex2base64 hex.c base64.c hex2base64.c

.PHONY: clean
clean:
	rm my-base64 hex2base64

.PHONY: test
test: my-base64
	shelltest . --color -p
	./random_test.sh
