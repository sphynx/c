CC = clang
CFLAGS = -g -Wall -Werror -Wextra -std=c99 -pedantic

vm: vm.o

.PHONY: clean run

clean:
	$(RM) -f *.o vm

run: vm
	./vm challenge.bin
