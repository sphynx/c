#!/bin/bash -x
CFLAGS="-g -Wall -Wextra -W -Werror --pedantic --std=c99 -O2"

gcc $CFLAGS -o my-base64 hex.c base64.c my-base64.c && shelltest . --color -p && ./random_test.sh
# gcc $CFLAGS -o hex2base64 hex.c base64.c hex2base64.c
