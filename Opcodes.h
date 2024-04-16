// Opcodes.h
#ifndef OPCODES_H
#define OPCODES_H

#include "stdbool.h"

typedef struct {
    char *name;
    int opcode;
    int sourceTypes[5]; // Array for allowed source operand types, -1 indicates no source operand allowed
    int destTypes[5];   // Array for allowed destination operand types, -1 indicates no destination operand allowed
} Opcode;

extern Opcode opcodes[];

int findOpcodeIndex(const char* command);
void initOperandTypes(int *types, int num, ...);
bool parseOperands(int* srcType, int* destType, const char* operands, Opcode opcode);
bool isValidType(int type, const int* allowedTypes);
bool isDirectIndexOperand(const char* operand);
int determineOperandType(const char* operand);

#endif

