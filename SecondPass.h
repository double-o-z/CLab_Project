// SecondPass.h
#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "Helper.h"
#include "FirstPassHelper.h"
#include "SecondPassHelper.h"
#include "Opcodes.h"

void SecondPass(ParsedFile* parsedFile, AssemblerState* state);
void processInstructionLine(AssemblerState* state, char* line, int lineNumber);
void processLineSecondPass(AssemblerState* state, char* line, int lineNumber);

#endif