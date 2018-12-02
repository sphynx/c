CC = clang
CFLAGS = -Wall -Wextra -Werror -O2 -pedantic -std=c99

BINARIES = d1-1 d1-2 d2-1

all: $(BINARIES)

.PHONY: clean run

clean:
	$(RM) -f *.o $(BINARIES)

run: d2-1
	@./d2-1 <input/2.txt
