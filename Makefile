TARGETS = vmtest
VM := vm.c vm.h
CC ?= clang
LEX ?= flex
CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wnested-externs -Winline -Wno-long-long  -Wunused-variable
CFLAGS += -Wstrict-prototypes -Werror -ansi -static -D_XOPEN_SOURCE
CFLAGS += -g -O0

.PHONY: all
all: $(TARGETS)

vmtest: $(VM) vmtest.c
	$(CC) $(CFLAGS) -o $@ vm.c vmtest.c

.PHONY: test
test: vmtest
	./vmtest && echo "All tests passed."

.PHONY: clean
clean:
	rm -f *.o $(TARGETS) core
