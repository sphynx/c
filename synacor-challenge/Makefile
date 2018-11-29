CC = clang
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic -O2

EXES = vm asm disasm orb

all: $(EXES)

vm: vm.o common.o
disasm: disasm.o common.o
asm: asm.o mpc.o common.o

%.s: %.bin disasm
	./disasm $< > $@

x.bin: challenge.bin
	./patch.sh

.PHONY: clean run walk beach run_orb

clean:
	$(RM) -f *.o $(EXES)

run: asm
	@./asm

walk: vm
	cat walkthrough - | ./vm challenge.bin

beach: vm x.bin
	cat walkthrough2 - | ./vm x.bin

run_orb: orb
	./orb
