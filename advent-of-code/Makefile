CC = clang
CFLAGS = -Wall -Wextra -Werror -O2 -pedantic -std=c99

BINARIES = d1-1 d1-2

all: $(BINARIES)

.PHONY: clean run

clean:
	$(RM) -f *.o $(BINARIES)

run: d1-2
	@./d1-2 <input/1.txt
