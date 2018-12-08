CC = clang
YACC = /usr/local/opt/bison/bin/bison
LEX = /usr/local/opt/flex/bin/flex

CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -O3
vpath %.a /usr/local/opt/flex/lib

EXES = vm asm disasm orb teleporter

all: $(EXES)

vm: vm.o common.o
disasm: disasm.o common.o
teleporter: teleporter.o

lex.yy.c: asm.l
	$(LEX) asm.l

asm.tab.c asm.tab.h: asm.y
	$(YACC) -d asm.y

lex.yy.o: asm.tab.h

asm: -lfl lex.yy.o asm.tab.o codegen.o table.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

my.challenge.bin: asm my.challenge.s
	./asm my.challenge.s my.challenge.bin

.PHONY: clean run walk carry-orb

clean:
	$(RM) -f *.o $(EXES) lex.yy.c asm.tab.c asm.tab.h test.bin

run: teleporter
	@./teleporter

walk: vm my.challenge.bin
	@cat walkthrough | ./vm my.challenge.bin

carry-orb: vm my.challenge.bin orb
	./orb >orb-path
	cat walkthrough2 orb-path | ./vm my.challenge.bin

test: asm disasm
	@./asm challenge.s my.challenge.bin
	@if diff -q challenge.bin my.challenge.bin; then echo "test passed"; fi
	@rm my.challenge.bin

