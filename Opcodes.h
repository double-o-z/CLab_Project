// Opcodes.h
#ifndef OPCODES_H
#define OPCODES_H

#include "Helper.h"
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
bool parseOperands(AssemblerState* state, int* srcType, int* destType, const char* operands, Opcode opcode);
Symbol* findSymbolInST(AssemblerState* state, const char* name);
bool isDirectIndexOperand(const char* operand);
int determineOperandType(AssemblerState* state, const char* operand);
bool isValidDirectOperand(const char* str);
bool isValidType(int type, const int* allowedTypes);

#endif

