/*  FirstPass.h */
#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "Helper.h"
#include "Opcodes.h"
#include "FirstPassHelper.h"

void firstPass(AssemblerState* state);
void processLine(AssemblerState* state);
void handleDefineDirective(AssemblerState* state, char* operands);
void handleDataDirective(AssemblerState* state, char* operands, const char* label);
void handleStringDirective(AssemblerState* state, char* operands, const char* label);
void handleExternalDirective(AssemblerState* state, char* operands);
void handleInstructions(AssemblerState* state, char* label, char* command, char* operands);

#endif