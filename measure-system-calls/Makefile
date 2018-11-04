CFLAGS = -g -Wall -Wextra -Werror -pedantic -std=c99

BINARIES = measure
TARGET = $(lastword $(BINARIES))

all: $(BINARIES)

.PHONY:	clean run

clean:
	$(RM) -r *.o $(BINARIES) *.dSYM

run: $(TARGET)
	./$(TARGET)

