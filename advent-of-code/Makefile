CC = clang
CFLAGS = -Wall -Wextra -Werror -O3 -pedantic -std=c99

BINARIES = d1-1 d1-2 d2-1 d2-2 d3-1 d3-2 d13-1

all: $(BINARIES)

.PHONY: clean run

clean:
	$(RM) -f *.o $(BINARIES)

run: d13-1
	@./d13-1 <input/13.txt
