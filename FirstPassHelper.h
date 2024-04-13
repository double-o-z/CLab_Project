#ifndef FIRSTPASSHELPER_H
#define FIRSTPASSHELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { MDEFINE, CODE, DATA } SymbolType;

typedef struct {
    char* label;
    SymbolType type;
    int value;
} Symbol;

typedef struct {
    int* instructions;
    int instructionCount;
    int* data;
    int dataCapacity;
    int dataCount;
    Symbol* symbols;
    int symbolsCapacity;
    int symbolsCount;
} AssemblerState;

void ensureCapacity(void** array, int currentSize, int newSize, size_t elementSize);
AssemblerState initAssemblerState();
char** splitFirstWhitespace(char* str);
void processLine(AssemblerState* state, char* line);

#endif /* FIRSTPASSHELPER_H */