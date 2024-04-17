// SecondPass.h
#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "Helper.h"
#include "FirstPassHelper.h"
#include "SecondPassHelper.h"
#include "Opcodes.h"

void SecondPass(ParsedFile* parsedFile, AssemblerState* state);
void processLineSecondPass(AssemblerState* state, char* line, int lineNumber);
void processInstructionLine(AssemblerState* state, char* command, char* operands, int lineNumber);

#endif