#ifndef __KRAM_ISA_H
#define __KRAM_ISA_H


#include <stdint.h>


const uint8_t VM_REG_SEL = 0x7;
const uint8_t VM_OP_SEL = 0xf8;


typedef enum e_reg {
	rA = 0,
	rX = 1,
	rY = 2,
	rSP = 3,
	rPC = 4,
	rFLG = 5
} register;


typedef enum e_instr {
	CMP_IMM = 0,
	BNE_IMM = 1,
	BEQ_IMM = 2,
	JMP_IMM = 3,
	MOV_IMM = 4,
	/* instruction 0b00101 isn't used */
	POKE_IMM = 6,
	PEEK_IMM = 7,
	CMP_REG = 8,
	BNE_REG = 9,
	BEQ_REG = 10,
	JMP_REG = 11,
	MOV_REG = 12,
	/* instruction 0b01101 isn't used */
	POKE_REG = 14,
	PEEK_REG = 15,
	ADD_IMM = 16,
	SUB_IMM = 17,
	MUL_IMM = 18,
	DIV_IMM = 19,
	AND_IMM = 20,
	OR_IMM = 21,
	NOT_IMM = 22,
	XOR_IMM = 23,
	ADD_REG = 24,
	SUB_REG = 25,
	MUL_REG = 26,
	DIV_REG = 27,
	AND_REG = 28,
	OR_REG = 29,
	NOT_REG = 30,
	XOR_REG = 31
} instr;

#endif
