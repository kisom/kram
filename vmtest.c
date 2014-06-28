#include <stdio.h>
#include <stdlib.h>

#include "vm.h"


int
main(void)
{
	VM	vm;
	uint8_t prog1[4] = "\x80\x02\x02";

	printf("Starting VM.\n");
	vm = vm_new();
	printf("VM started.\n");

	printf("Loading program.\n");
	vm_load(vm, prog1, 3);

	printf("Running program.\n");
	vm_run(vm);

	printf("Result: %d\n", vm_result(vm));

	printf("Stopping VM.\n");
	vm_destroy(vm);
	printf("VM stopped.\n");

	return EXIT_SUCCESS;
}
