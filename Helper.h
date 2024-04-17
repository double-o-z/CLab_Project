#ifndef HELPER_H
#define HELPER_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern const int INDEX_FIRST_INSTRUCTION;

typedef struct {
    char** lines;
    int numberOfLines;
    const char* fileName;
} ParsedFile;

typedef enum { MDEFINE, CODE, DATA, EXTERNAL, ENTRY } SymbolType;

typedef struct {
    char* label;
    SymbolType type;
    int value;
} Symbol;

typedef struct {
    int* array;     // Array of data or instructions
    int count;      // Counter for number of elements (DC or IC)
} DynamicArray;

typedef struct {
    DynamicArray instructions;  // Instructions array with IC
    DynamicArray data;          // Data array with DC
    Symbol* symbols;            // Symbol table
    int symbolsCapacity;        // Capacity of the symbol table
    int symbolsCount;           // Number of symbols currently stored
    bool duplicateSymbols;      // Flag to indicate if duplicate symbols have been found
    int instructionCounter;     // Counter for the current instruction in the second pass
} AssemblerState;

void printAllLines(ParsedFile parsedFile);
void dynamicInsert(DynamicArray* array, int value);
AssemblerState initAssemblerState();
void printSymbolsTable(const AssemblerState* state);

#endif /* HELPER_H */