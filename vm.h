#ifndef __KRAM_VM_H
#define __KRAM_VM_H


#include <stdint.h>


#define VM_SP_START	0x200
#define VM_DEFAULT_MEM	0x400

#define VM_OK	1
#define VM_STOP	2
#define VM_ERR	3

#define VM_FLAG_CMP	1


struct s_reg {
	uint8_t		A;
	uint8_t		X;
	uint8_t		Y;
	uint16_t	SP;
	uint16_t	PC;
	uint8_t		FLG;
};

typedef struct s_vm {
	uint8_t		*ram;
	struct s_reg	 regs;
} * VM;


VM	vm_new(void);
void	vm_destroy(VM);
int	vm_run(VM);
void	vm_load(VM, uint8_t *, size_t);
uint8_t	vm_result(VM);
void	vm_dump_registers(VM);


#endif
