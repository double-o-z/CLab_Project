#ifndef SECONDPASSHELPER_H
#define SECONDPASSHELPER_H
#define _POSIX_C_SOURCE 200809L
#include "Helper.h"
#include "FirstPassHelper.h"
#include <stdbool.h>
#include <ctype.h>

// Operand placement identifiers
typedef enum { SOURCE_OPERAND, DESTINATION_OPERAND } OperandPlacement;

bool isDirective(char* line);
void encodeOperandDataWords(AssemblerState* state, int srcType, int destType, char* operands, int lineNumber);
void encodeRegisterPair(AssemblerState* state, char* srcOperand, char* destOperand, int lineNumber);
void encodeOperandByType(AssemblerState* state, int type, char* operand, int lineNumber, OperandPlacement placement);
void encodeImmediateOperand(AssemblerState* state, char* operands, int lineNumber, bool isSubOperand);
void encodeDirectOperand(AssemblerState* state, char* operands, int lineNumber);
void encodeDirectIndexOperand(AssemblerState* state, char* operands, int lineNumber);
void encodeRegisterOperand(AssemblerState* state, char* operand, int lineNumber, OperandPlacement placement);
int calculateRegisterPairWord(const char* srcOperand, const char* destOperand);
int calculateRegisterWord(const char* operand, OperandPlacement placement);
Symbol* findSymbolInST(AssemblerState* state, const char* name);
int calculateImmediateWord(int value);
int calculateDirectWord(int value, SymbolType type);
#endif /* SECONDPASSHELPER_H */