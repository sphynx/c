#!/bin/zsh -x
CFLAGS=-DUSE_MY_GETLINE
gcc -g -Wall -W -Werror ${CFLAGS} --pedantic -o my-grep my-grep.c && shelltest my-grep.test --color
