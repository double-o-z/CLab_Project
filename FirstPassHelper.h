#ifndef FIRSTPASSHELPER_H
#define FIRSTPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Helper.h"

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
} AssemblerState;

AssemblerState initAssemblerState();
void dynamicInsert(DynamicArray* array, int value);
void dynamicInsertSymbol(AssemblerState* state, Symbol newSymbol);
int findSymbolValue(const AssemblerState* state, const char* label);

void printSymbolsTable(const AssemblerState* state);
void printDataList(const AssemblerState* state);
void printInstructionsList(const AssemblerState* state);
void intToBinaryString(int value, char *buffer);

char** splitFirstWhitespace(char* str);
char* trim(char* str);
int isValidInteger(const char* str);
int to14BitTwosComplement(int value);
int createFirstWord(int srcType, int destType, int opcode);
void addInstructionToInstructionList(DynamicArray* array, int instructionWord);
void prepareOperandDataWords(AssemblerState* state, int srcType, int destType);
const char* operandTypeToString(int type);
void printOperandTypes(int srcType, int destType);

#endif /* FIRSTPASSHELPER_H */