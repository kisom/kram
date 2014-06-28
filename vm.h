#ifndef __KRAM_VM_H
#define __KRAM_VM_H


#include <stdint.h>

/*
 * Programs default to starting at address 0.
 */
const uint16_t		VM_ENTRY_POINT = 0;

 /*
  * default stack is 128B starting at 0x200.
  */
const uint16_t		VM_SP_START = 0x200;

/*
 * The data segment is 128B starting at 0x200.
 */
const uint16_t		VM_DS_START = 0x100;

/*
 * Virtual machines default to a memory size of 1K.
 */
const uint16_t		VM_MEMORY = 0x400;


#define VM_OK	1
#define VM_STOP	2
#define VM_ERR	3

/*
 * This flag is set when a compare is equal.
 */
#define VM_FLAG_CMP	1


/*
 * KRAM has three GP registers; see the architecture docs
 * for more information.
 */
struct s_reg {
	uint8_t		A;
	uint8_t		X;
	uint8_t		Y;
	uint16_t	SP;
	uint16_t	PC;
	uint8_t		FLG;
};


/*
 * A VM has some memory and a register set.
 */
typedef struct s_vm {
	uint8_t		*ram;
	struct s_reg	 regs;
} * VM;


VM	vm_new(void);
VM	vm_new_with(uint16_t, uint16_t, uint16_t);
void	vm_destroy(VM);
int	vm_run(VM);
void	vm_load(VM, uint8_t *, size_t);
uint8_t	vm_result(VM);
void	vm_dump_registers(VM);


#endif
