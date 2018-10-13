#!/bin/bash -x
CFLAGS="-g -Wall -W -Werror --pedantic"
gcc $CFLAGS -o my-base64 base64.c && shelltest . --color -p
