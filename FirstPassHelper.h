#ifndef FIRSTPASSHELPER_H
#define FIRSTPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void dynamicInsertSymbol(AssemblerState* state, Symbol newSymbol);
void dynamicInsertExternal(AssemblerState* state, External newExternal);
int findSymbolValue(AssemblerState* state, const char* label);

void printDataList(AssemblerState* state);
void printInstructionsList(AssemblerState* state);
void intToBinaryString(int value, char *buffer);

char** splitFirstWhitespace(char* str);
char* trim(char* str);
int to14BitTwosComplement(int value);
int createFirstWord(AssemblerState* state, int srcType, int destType, int opcode);
void addInstructionToInstructionList(AssemblerState* state, DynamicArray* array, int instructionWord);
void prepareOperandDataWords(AssemblerState* state, int srcType, int destType);
const char* operandTypeToString(int type);
void printOperandTypes(int srcType, int destType);

#endif /* FIRSTPASSHELPER_H */
