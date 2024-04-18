// FirstPass.h
#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "Helper.h"
#include "Opcodes.h"
#include "FirstPassHelper.h"

void FirstPass(AssemblerState* state);
void processLine(AssemblerState* state, char* line, int lineNumber);
void handleDefineDirective(AssemblerState* state, char* operands, int lineNumber, const char* fullLine);
void handleDataDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
void handleStringDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
void handleExternalDirective(AssemblerState* state, char* operands, int lineNumber, const char* line);
void handleInstructions(AssemblerState* state, char* label, char* command, char* operands, int lineNumber);

#endif