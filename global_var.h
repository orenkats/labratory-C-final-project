#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include "macro.h"



typedef enum booleans {
	FALSE = 0, TRUE = 1
}
booleans;

typedef enum addressing_types {
	immd_addr = 1,
	direct_addr = 3,
	reg_direct_addr = 5,
	no_addr = -1
} 
addressing_type;

typedef enum symbol_type {
	CODE,
	DATA,
	STRING
} symbol_type;

typedef enum {
	ENTRY,
	EXTERN,
	NONE

} entry_extern;

struct entryExtern {
char name[MAX_NAME_LENGTH];
 char status[MAX_NAME_LENGTH];
struct entryExtern* nextentryExtern;
};
struct symTable {
	char symName[MAX_NAME_LENGTH];
	int symValue;
	symbol_type symType;
	entry_extern entryExternFlag;   
	struct symTable* nextSym;
};
typedef enum opcodes {
	
	mov = 0,
	cmp = 1,
	add = 2,
	sub = 3,
	not = 4,
	clr = 5,
	lea = 6,
	inc = 7,
	dec = 8,
	jmp = 9,
	bne = 10,
	red = 11,
	prn = 12,
	jsr = 13,
	rts = 14,
	stop = 15,	
	no_op = -1
} 
opcode;


typedef struct machine_instruction {
	unsigned short instruction;  /* Add this field to store the full instruction*/
	opcode opcode;
	unsigned int src_addr_mode;
	unsigned int dest_addr_mode;
	unsigned int ARE;
} machine_instruction;

typedef struct data_word {
	unsigned int ARE : 2;           /* A, R, E bits (2 bits)*/
	unsigned int operand : 10;      /* Operand data or register number (10 bits)*/
	unsigned short data;            /* Add this field to store the full data word*/
} data_word;

typedef struct {
	char operation[50];
	char operand1[50];
	char operand2[50];
} ParsedInstruction;

#endif

