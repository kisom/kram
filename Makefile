TARGETS = kramvm
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

kramvm: vm.o kramvm.c
	$(CC) $(CFLAGS) -o $@ vm.o kramvm.c

.PHONY: test
test: vmtest
	./vmtest && echo "All tests passed."

.PHONY: clean
clean:
	rm -f *.o $(TARGETS) core
