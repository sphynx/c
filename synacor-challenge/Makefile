CC = clang
YACC = /usr/local/opt/bison/bin/bison
LEX = /usr/local/opt/flex/bin/flex

CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic -O2
vpath %.a /usr/local/opt/flex/lib

EXES = vm asm disasm orb

all: $(EXES)

vm: vm.o common.o
disasm: disasm.o common.o

lex.yy.c: asm.l
	$(LEX) asm.l

asm.tab.c asm.tab.h: asm.y
	$(YACC) -d asm.y

lex.yy.o: asm.tab.h

asm: -lfl lex.yy.o asm.tab.o codegen.o table.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

%.s: %.bin disasm
	./disasm $< > $@

x.bin: challenge.bin
	./patch.sh

.PHONY: clean run walk beach run_orb

clean:
	$(RM) -f *.o $(EXES) lex.yy.c asm.tab.c asm.tab.h test.bin

run: asm
	@./asm

walk: vm my.challenge.bin
	cat walkthrough - | ./vm my.challenge.bin

beach: vm x.bin
	cat walkthrough2 - | ./vm x.bin

run_orb: orb
	./orb

my.challenge.bin: asm challenge.s
	./asm challenge.s my.challenge.bin

test: asm disasm
	@./asm challenge.s my.challenge.bin
	@if diff -q challenge.bin my.challenge.bin; then echo "test passed"; fi
	@rm my.challenge.bin

