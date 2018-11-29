CC = clang
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic -O2

all: vm disasm orb

vm: vm.o common.o
disasm: disasm.o common.o

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

run_orb: orb
	./orb
