CC = clang
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic

all: vm disasm

vm: vm.o
disasm: disasm.o

.PHONY: clean run

clean:
	$(RM) -f *.o vm

run: vm
	./vm challenge.bin

walk: vm
	cat walkthrough - | ./vm challenge.bin

dis: disasm
	./disasm challenge.bin
