#ifndef __KRAM_VM_H
#define __KRAM_VM_H


#include <stdint.h>


#define VM_SP_START	0x200
#define VM_DEFAULT_MEM	0x400


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


#endif
