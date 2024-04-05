// Opcodes.h
#ifndef OPCODES_H
#define OPCODES_H

typedef enum {
    MOV = 0, CMP, ADD, SUB, NOT, CLR, LEA, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, HLT
} Opcode;

extern const char* OpcodeCommands[];

#endif

