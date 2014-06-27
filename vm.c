#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

#include "vm.h"


VM
vm_new()
{
	VM	vm;

	vm = calloc(1, sizeof(struct s_vm));
	if (vm == NULL)
		abort();

	vm->ram = calloc(VM_DEFAULT_MEM, sizeof(uint8_t));
	if (vm->ram == NULL)
		abort();

	return vm;
}


void
vm_destroy(VM vm)
{
	free(vm->ram);
	free(vm);
}
