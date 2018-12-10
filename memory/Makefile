CC = gcc
CPPFLAGS = -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2
EXES = memory-user null

all: $(EXES)

null: null.o

null.o: null.c
	$(CC) -g -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(EXES) *.o
