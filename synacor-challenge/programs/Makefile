ASM = ../asm
VM = ../vm
CPP = cc -E -P

all: run

hello.i: hello.s io.s
	$(CPP) hello.s >$@

hello.exe: hello.i
	$(ASM) hello.i hello.exe

.PHONY: run clean

run: hello.exe
	$(VM) hello.exe

clean:
	$(RM) hello.i hello.exe
