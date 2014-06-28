#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "isa.h"
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


static uint8_t
register_value(VM vm, uint8_t sel)
{
	sel &= VM_REG_SEL;
	switch (sel) {
	case rA:
		return vm->regs.A;
	case rX:
		return vm->regs.X;
	case xY:
		return vm->regs.Y;
	case rSP:
		return (uint8_t)vm->regs.SP;
	case rPC:
		return (uint8_t)vm->regs.PC;
	case rFLG:
		return vm->regs.FLG;
	default:
		fprintf(stderr, "Invalid register %d\n", sel);
		abort();
	}
}


static void
register_set(VM vm, uint8_t sel, uint8_t val) {
	sel &= VM_REG_SEL;
	switch (sel) {
	case 0:
		vm->regs.A = val;
	case 1:
		vm->regs.X = val;
	case 2:
		vm->regs.Y = val;
	case 3:
		vm->regs.SP = (uint16_t)val;
	case 4:
		vm->regs.PC = (uint16_t)val;
	case 5:
		vm->regs.FLG = val;
	default:
		fprintf(stderr, "Invalid register %d\n", sel);
		abort();
	}

}


static uint8_t
vm_next8(VM vm) {
	return vm->ram[vm->regs.PC++];
}


static uint16_t
vm_next16(VM vm) {
	uint16_t	val;

	val = (uint16_t)(vm->ram[vm->regs.PC++]) << 8;
	val += (vm->ram[vm->regs.PC++]);
	return val;
}


static int
vm_cmp(VM vm, uint8_t op)
{
	uint8_t	a, b;

	if ((op >> 3) & VM_REG_SEL) {
		a = register_value(vm, op & VM_REG_SEL);
	} else {
		a = vm->ram[vm->regs.PC++];
	}
	b = vm->ram[vm->regs.PC++];

	if (a == b) {
		vm->regs.FLG |= VM_FLAG_CMP;
	} else {
		vm->regs.FLG &= ~VM_FLAG_CMP;
	}
	return VM_OK;
}


static int
vm_bne(VM vm, uint8_t op)
{
	uint16_t a;

	if ((op >> 3) & VM_REG_SEL) {
		a = (uint16_t)vm->regs.X << 8;
		a += (uint16_t)vm->regs.Y;
	} else {
		a = vm_next16(vm);
	}

	if (!vm->regs.FLG & VM_FLAG_CMP)
		vm->regs.PC = a;
	return VM_OK;
}


static int
vm_beq(VM vm, uint8_t op)
{
	uint16_t a;

	if ((op >> 3) & VM_REG_SEL) {
		a = (uint16_t)vm->regs.X << 8;
		a += (uint16_t)vm->regs.Y;
	} else {
		a = vm_next16(vm);
	}

	if (vm->regs.FLG & VM_FLAG_CMP)
		vm->regs.PC = a;
	return VM_OK;
}


static int
vm_jmp(VM vm, uint8_t op)
{
	uint16_t a;

	if ((op >> 3) & VM_REG_SEL) {
		a = (uint16_t)vm->regs.X << 8;
		a += (uint16_t)vm->regs.Y;
	} else {
		a = vm_next16(vm);
	}

	vm->regs.PC = a;
	return VM_OK;
}


static int
vm_move(VM vm, uint8_t op)
{
	uint8_t	src;

	if ((op >> 3) & VM_REG_SEL) {
		src = register_value(vm, vm_next8(vm));
	} else {
		src = vm_next8(vm);
	}
	register_set(vm, op, src);
	return VM_OK;
}


static int
vm_poke(VM vm, uint8_t op)
{
	uint16_t a;

	if ((op >> 3) & VM_REG_SEL) {
		a = (uint16_t)vm->regs.X << 8;
		a += (uint16_t)vm->regs.Y;
	} else {
		a = vm_next16(vm);
	}

	register_set(vm, op, 
	return VM_OK;
}


static int
vm_peek(VM vm, uint8_t op) { return VM_ERR; }


static int
control_step(VM vm, uint8_t op)
{
	switch (op >> 3) {
	case CMP_IMM:
	case CMP_REG:
		return vm_cmp(vm, op);
	case BNE_IMM:
	case BNE_REG:
		return vm_bne(vm, op);
	case BEQ_IMM:
	case BEQ_REG:
		return vm_beq(vm, op);
	case JMP_IMM:
	case JMP_REG:
		return vm_jmp(vm, op);
	case MOV_IMM:
	case MOV_REG:
		return vm_move(vm, op);
	case PEEK_IMM:
	case PEEK_REG:
		return vm_peek(vm, op);
	case POKE_IMM:
	case POKE_REG:
		return vm_poke(vm, op);
	default:
		fprintf(stderr, "Unknown control instruction.\n");
		abort();
	}
}


static int
vm_step(VM vm)
{
	uint8_t	op;

	op = vm->ram[vm->regs.PC++];
	if ((op >> 3) < 16) {
		return control_step(vm, op);
	}
	return VM_STOP;
}


int
vm_run(VM vm)
{
	int	res;

	while (VM_OK == (res = vm_step(vm))) ;

	return res;
}
