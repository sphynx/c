#!/bin/bash -x
CFLAGS="-g -Wall -W -Werror --pedantic"
gcc $CFLAGS -o my-zip my-zip.c && \
    gcc $CFLAGS -o my-unzip my-unzip.c && \
    shelltest tests/zip.test --color --execdir
