CC = clang
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic

all: vm disasm

vm: vm.o
disasm: disasm.o

%.s: %.bin disasm
	./disasm $< > $@

x.bin: challenge.bin
	./patch.sh

.PHONY: clean run walk

clean:
	$(RM) -f *.o vm

run: vm
	./vm challenge.bin

walk: vm
	cat walkthrough - | ./vm challenge.bin

beach: vm x.bin
	cat walkthrough2 - | ./vm x.bin


