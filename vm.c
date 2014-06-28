#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "isa.h"
#include "vm.h"


/*
 * vm_new creates a new VM with the default memory parameters.
 */
VM
vm_new()
{
	VM	vm;

	vm = calloc(1, sizeof(struct s_vm));
	if (vm == NULL)
		abort();

	vm->ram = calloc(VM_MEMORY, sizeof(uint8_t));
	if (vm->ram == NULL)
		abort();

	vm->regs.SPA = VM_SP_START>>8;
	vm->regs.SPB = VM_SP_START&0xFF;
	vm->regs.PC = VM_ENTRY_POINT;
	return vm;
}


VM
vm_new_with(uint16_t memory, uint16_t sp, uint16_t entry)
{
	VM	vm;

	vm = calloc(1, sizeof(struct s_vm));
	if (vm == NULL)
		abort();

	vm->ram = calloc(memory, sizeof(uint8_t));
	if (vm->ram == NULL)
		abort();

	vm->regs.SPA = sp>>8;
	vm->regs.SPB = sp&0xFF;
	vm->regs.PC = entry;
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
	sel &= VM_REG_MASK;
	switch (sel) {
	case rA:
		return vm->regs.A;
	case rX:
		return vm->regs.X;
	case rY:
		return vm->regs.Y;
	case rSPA:
		return vm->regs.SPA;
	case rPC:
		return (uint8_t)vm->regs.PC;
	case rFLG:
		return vm->regs.FLG;
	case rB:
		return vm->regs.B;
	case rSPB:
		return vm->regs.SPB;
	default:
		fprintf(stderr, "[rval] nvalid register %d\n", sel);
		abort();
	}
}


static void
register_set(VM vm, uint8_t sel, uint8_t val) {
	sel &= VM_REG_MASK;
	switch (sel) {
	case rA:
		vm->regs.A = val;
		break;
	case rX:
		vm->regs.X = val;
		break;
	case rY:
		vm->regs.Y = val;
		break;
	case rSPA:
		vm->regs.SPA = val;
		break;
	case rPC:
		vm->regs.PC = (uint16_t)val;
		break;
	case rFLG:
		vm->regs.FLG = val;
		break;
	case rB:
		vm->regs.B = val;
		break;
	case rSPB:
		vm->regs.SPB = val;
		break;
	default:
		fprintf(stderr, "[rset] Invalid register %d\n", sel);
		abort();
	}

}


static uint16_t
register_address(VM vm)
{
	uint16_t	addr;

	addr = ((uint16_t)vm->regs.X) << 8;
	addr += vm->regs.Y;
	return addr;
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

	a = register_value(vm, op);
	b = vm_next8(vm);

	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

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

	if (op & VM_REG_SEL) {
		a = register_address(vm);
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

	if (op & VM_REG_SEL) {
		a = register_address(vm);
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

	if (op & VM_REG_SEL) {
		a = register_address(vm);
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

	if (op & VM_REG_SEL) {
		src = vm_next8(vm) & VM_REG_MASK;
		src = register_value(vm, src);
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

	if (op & VM_REG_SEL) {
		a = register_address(vm);
	} else {
		a = vm_next16(vm);
	}

	vm->ram[a] = register_value(vm, op);
	return VM_OK;
}


static int
vm_peek(VM vm, uint8_t op)
{
	uint16_t	a;
	uint8_t		val;

	if (op & VM_REG_SEL) {
		a = register_address(vm);
	} else {
		a = vm_next16(vm);
	}

	val = vm->ram[a];
	register_set(vm, rA, val);
	return VM_OK;
}


static int
vm_add(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a+b);
	return VM_OK;
}


static int
vm_sub(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a-b);
	return VM_OK;
}


static int
vm_mul(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a*b);
	return VM_OK;
}


static int
vm_div(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a/b);
	register_set(vm, rB, a%b);
	return VM_OK;
}


static int
vm_and(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a&b);
	return VM_OK;
}


static int
vm_not(VM vm, uint8_t op)
{
	uint8_t a;

	if (op & VM_REG_SEL) {
		a = register_value(vm, op);
	} else {
		a = vm_next8(vm);
	}

	register_set(vm, op, ~a);
	return VM_OK;
}


static int
vm_or(VM vm, uint8_t op)
{
	uint8_t a, b;

	a = register_value(vm, op);
	b = vm_next8(vm);
	if (op & VM_REG_SEL) {
		b = register_value(vm, b);
	}

	register_set(vm, rA, a|b);
	return VM_OK;
}


static int
vm_xor(VM vm, uint8_t op)
{
	uint8_t a, b;

	if (op & VM_REG_SEL) {
		a = register_value(vm, op);
		b = vm_next8(vm);
	} else {
		a = vm_next8(vm);
		b = vm_next8(vm);
	}

	register_set(vm, rA, a^b);
	return VM_OK;
}


static int
do_syscall(VM vm)
{
	syscall		i = (syscall)(register_value(vm, rA));
	uint16_t	address = 0;

	switch (i) {
	case scExit:
		return VM_STOP;
	case scPrintString:
		address = register_address(vm);
		printf("%s", (unsigned char *)(vm->ram + address));
		return VM_OK;
	case scPrintNum:
		address = register_address(vm);
		printf("%d", vm->ram[address]);
		return VM_OK;
	default:
		fprintf(stderr, "Unknown syscall.\n");
		abort();
	}
}


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
	case SYSCALL:
		return do_syscall(vm);
	default:
		fprintf(stderr, "Unknown control instruction %d.\n", op >> 3);
		fprintf(stderr, "\toperand: %d\n", op);
		return VM_ERR;
	}
}


static int
oper_step(VM vm, uint8_t op)
{
	switch (op >> 3) {
	case ADD_IMM:
	case ADD_REG:
		return vm_add(vm, op);
	case SUB_IMM:
	case SUB_REG:
		return vm_sub(vm, op);
	case MUL_IMM:
	case MUL_REG:
		return vm_mul(vm, op);
	case DIV_IMM:
	case DIV_REG:
		return vm_div(vm, op);
	case AND_IMM:
	case AND_REG:
		return vm_and(vm, op);
	case NOT_IMM:
	case NOT_REG:
		return vm_not(vm, op);
	case OR_IMM:
	case OR_REG:
		return vm_or(vm, op);
	case XOR_IMM:
	case XOR_REG:
		return vm_xor(vm, op);
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
	} else {
		return oper_step(vm, op);
	}
	return VM_STOP;
}


int
vm_run(VM vm)
{
	int	res;

	while (VM_OK == (res = vm_step(vm))) ;
	/*	vm_dump_registers(vm); */

	if (VM_ERR == res)
		vm_dump_registers(vm);
	return res;
}


void
vm_load(VM vm, uint8_t *prog, size_t prog_len)
{
	memcpy(vm->ram, prog, prog_len);
}


uint8_t
vm_result(VM vm)
{
	return register_value(vm, rA);
}


void
vm_dump_registers(VM vm)
{
	printf("Registers:\n");
	printf("\t  A: 0x%x\n", vm->regs.A);
	printf("\t  B: 0x%x\n", vm->regs.B);
	printf("\t  X: 0x%x\n", vm->regs.X);
	printf("\t  Y: 0x%x\n", vm->regs.Y);
	printf("\tSPA: 0x%x\n", vm->regs.SPA);
	printf("\tSPB: 0x%x\n", vm->regs.SPB);
	printf("\t PC: 0x%x\n", vm->regs.PC);
	printf("\tFLG: 0x%x\n", vm->regs.FLG);
}
