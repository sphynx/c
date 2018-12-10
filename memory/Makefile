CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=c99 -O2
CPPFLAGS = -D_XOPEN_SOURCE=700
EXES = memory-user null not-freed

all: $(EXES)

null: null.o

null.o: null.c
	$(CC) -g -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(EXES) *.o
