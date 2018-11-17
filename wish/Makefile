CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

all: wish

wish: wish.o list.o

.PHONY: clean run

run: wish
	@./wish

clean:
	$(RM) -r -f wish *.o *.dSYM
