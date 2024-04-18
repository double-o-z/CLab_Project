#ifndef SECONDPASSHELPER_H
#define SECONDPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include "Helper.h"
#include "FirstPassHelper.h"
#include "Opcodes.h"
#include <stdbool.h>
#include <ctype.h>

// Operand placement identifiers
typedef enum { SOURCE_OPERAND, DESTINATION_OPERAND } OperandPlacement;

bool isDirective(char* line);
void encodeOperandDataWords(AssemblerState* state, int srcType, int destType, char* operands);
void encodeRegisterPair(AssemblerState* state, char* srcOperand, char* destOperand);
void encodeOperandByType(AssemblerState* state, int type, char* operand, OperandPlacement placement);
void encodeImmediateOperand(AssemblerState* state, char* operands, bool isSubOperand);
void encodeDirectOperand(AssemblerState* state, char* operands);
void addExternalLocation(AssemblerState* state, char* label);
void encodeDirectIndexOperand(AssemblerState* state, char* operands);
void encodeRegisterOperand(AssemblerState* state, char* operand, OperandPlacement placement);
int calculateRegisterPairWord(const char* srcOperand, const char* destOperand);
int calculateRegisterWord(const char* operand, OperandPlacement placement);
int calculateImmediateWord(int value);
int calculateDirectWord(int value, SymbolType type);
#endif /* SECONDPASSHELPER_H */