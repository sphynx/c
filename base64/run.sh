#!/bin/bash -x
CFLAGS="-g -Wall -Wextra -W -Werror --pedantic --std=c99 -O2"
gcc $CFLAGS -o my-base64 base64.c && shelltest . --color -p
