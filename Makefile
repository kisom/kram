TARGETS = libvm.a kramvm
VM := vm.c vm.h
CC = clang
CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wnested-externs -Winline -Wno-long-long  -Wunused-variable
CFLAGS += -Wstrict-prototypes -Werror
CFLAGS += -std=c99 -static -D_XOPEN_SOURCE -g -O0

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: all
all: $(TARGETS)

libvm.a: $(VM) vm.o
	ar -crs $@ vm.o

kramvm: libvm.a kramvm.c
	$(CC) $(CFLAGS) -o $@ kramvm.c libvm.a

.PHONY: test
test: vmtest
	./vmtest && echo "All tests passed."

.PHONY: clean
clean:
	rm -f *.o $(TARGETS) core
