#ifndef FIRSTPASSHELPER_H
#define FIRSTPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Helper.h"

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
void processLine(AssemblerState* state, char* line, int lineNumber);
char* trim(char* str);
void handleDefineDirective(AssemblerState* state, char* operands, int lineNumber, const char* fullLine);
int isValidInteger(const char* str);
int findSymbolValue(const AssemblerState* state, const char* label);
void handleDataDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
void printSymbolsTable(const AssemblerState* state);
void printDataList(const AssemblerState* state);

#endif /* FIRSTPASSHELPER_H */