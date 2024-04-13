// Opcodes.h
#ifndef OPCODES_H
#define OPCODES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* name;           // Opcode name
    int opcode;           // Opcode value
    char* sourceTypes;    // Valid source operand types
    char* destTypes;      // Valid destination operand types
} Opcode;

#define OPCODE_COUNT 16  // Total number of opcodes

extern Opcode opcodes[]; // Declaration only

#endif

