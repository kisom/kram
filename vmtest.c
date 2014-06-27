#include <stdio.h>
#include <stdlib.h>

#include "vm.h"


int
main(void)
{
	VM	vm;

	printf("Starting VM.\n");
	vm = vm_new();
	printf("VM started.\nStopping VM.\n");
	vm_destroy(vm);
	printf("VM stopped.\n");

	return EXIT_SUCCESS;
}
