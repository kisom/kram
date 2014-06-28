/*
 * Copyright (c) 2014 Kyle Isom <kyle@tyrfingr.is>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>

#include "vm.h"


uint16_t memory = VM_MEMORY;
uint16_t sp = VM_SP_START;
uint16_t entry = VM_ENTRY_POINT;


static int	debug = 0;


static int
interpreter(void)
{
	fprintf(stderr, "The interpreter is not implemented yet.\n");
	return EXIT_FAILURE;
}


static int
run(const char *filespec)
{
	struct stat	 st;
	int		 i;
	int		 res;
	int		 status = EXIT_FAILURE;
	VM		 vm = NULL;
	FILE		*bin = NULL;
	uint8_t		*prog = NULL;

	if (-1 == stat(filespec, &st))
		goto exit;

	printf("Loading %lu byte program.\n", st.st_size);
	if (st.st_size > memory) {
		fprintf(stderr, "Out of memory (prog=%uB, vm=%uB).\n",
		    (unsigned)st.st_size, memory);
		return EXIT_FAILURE;
	}

	prog = malloc(st.st_size);
	if (NULL == prog)
		goto exit;

	if (NULL == (bin = fopen(filespec, "rb")))
		goto exit;

	if (st.st_size != (off_t)fread(prog, sizeof(uint8_t), st.st_size, bin))
		goto exit;

	vm = vm_new_with(memory, sp, entry);
	if (NULL == vm)
		goto exit;

	printf("Starting VM.\n");
	vm_load(vm, prog, st.st_size);
	for (i = 0; i < 72; i++) printf("-");
	printf("\n");
	res = vm_run(vm);
	for (i = 0; i < 72; i++) printf("-");
	printf("\n");
	if (VM_ERR == res)
		goto exit;

	printf("OK\n");
	status = EXIT_SUCCESS;

exit:
	if (prog)
		free(prog);

	if (vm) {
		if (debug) {
			vm_dump_registers(vm);
		}
		vm_destroy(vm);
	}

	if (bin)
		fclose(bin);
	return status;
}



int
main(int argc, char *argv[])
{
	int		opt;
	uint16_t	val;

	while (-1 != (opt = getopt(argc, argv, "de:m:s:"))) {
		switch (opt) {
		case 'd':
			debug = 1;
			break;
		case 'e':
			val = (uint16_t)atoi(optarg);
			entry = val;
			break;
		case 'm':
			val = (uint16_t)atoi(optarg);
			memory = val;
			break;
		case 's':
			val = (uint16_t)atoi(optarg);
			sp = val;
			break;
		default:
			fprintf(stderr, "Unrecognised option.\n");
			return EXIT_FAILURE;
		}
	}

	argv += optind;
	argc -= optind;

	if (0 == argc)
		return interpreter();
	else
		return run(argv[0]);
}
