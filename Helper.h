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
    int currentLineNum;
} ParsedFile;

typedef enum { MDEFINE, CODE, DATA, EXTERNAL, ENTRY } SymbolType;

typedef struct {
    char* label;
    SymbolType type;
    int value;
} Symbol;

typedef struct {
    char* label;
    int lineNumber;
} External;

typedef struct {
    int* array;     // Array of data or instructions
    int count;      // Counter for number of elements (DC or IC)
} DynamicArray;

typedef struct {
    const char* inputFilename;  // The current file name without extension.
    DynamicArray instructions;  // Instructions array with IC
    DynamicArray data;          // Data array with DC
    Symbol* symbols;            // Symbol table
    int symbolsCount;           // Number of symbols currently stored
    bool assemblerError;        // Flag that tells whether assembler can create output or not.
    int instructionCounter;     // Counter for the current instruction in the second pass
    External* externals;        // Externals array.
    int externalsCount;         // Number of externals currently stored
    bool entriesExist;          // To know whether at least one entry directive exists.
    bool debugMode;             // Debug mode flag
    ParsedFile parsedFile;      // The contents of the input file after parsing.
} AssemblerState;

AssemblerState initAssemblerState(const char* inputFilename, bool debugMode);
void dynamicInsert(AssemblerState* state, DynamicArray* array, int value);

void printAllLines(AssemblerState* state);
void printSymbolsTable(AssemblerState* state);
void printExternalsTable(AssemblerState* state);

bool isValidInteger(const char* str);
const char* symbolTypeToString(int type);

#endif /* HELPER_H */