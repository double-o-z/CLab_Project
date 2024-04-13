#ifndef FIRSTPASSHELPER_H
#define FIRSTPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Helper.h"
#include "Opcodes.h"

typedef enum { MDEFINE, CODE, DATA } SymbolType;

typedef struct {
    char* label;
    SymbolType type;
    int value;
} Symbol;

typedef struct {
    int instructionIndex;  // Index of the instruction in the instruction list
    int lineNumber;        // Line number of the instruction for error reporting
    char* line;            // Copy of the original line for reference
} UnresolvedInstruction;

typedef struct {
    int* instructions;
    int instructionCount;
    int* data;
    int dataCapacity;
    int dataCount;
    Symbol* symbols;
    int symbolsCapacity;
    int symbolsCount;
    UnresolvedInstruction* unresolved;
    int unresolvedCount;
    int unresolvedCapacity;
} AssemblerState;

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
void handleStringDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
int resolveRegister(const char* operand);
int getOperandValue(AssemblerState* state, const char* operand);
int resolveOperand(AssemblerState* state, char* operand, int* resolvedValue);
void insertWords(AssemblerState* state, Opcode* cmd, int srcType, int srcValue, int dstType, int dstValue);
void addUnresolved(AssemblerState* state, int instructionIndex, int lineNumber, char* line);
void finalCheck(AssemblerState* state);
Opcode* findOpcode(const char* command);
void processInstruction(AssemblerState* state, char* command, char* operands, int lineNumber);
void addSymbolAndUpdateUnresolved(AssemblerState* state, Symbol newSymbol);
int resolveInstruction(AssemblerState* state, UnresolvedInstruction* unresolved);
#endif /* FIRSTPASSHELPER_H */