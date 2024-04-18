/*  SecondPass.h */
#ifndef SECONDPASS_H
#define SECONDPASS_H
#include "Helper.h"
#include "FirstPassHelper.h"
#include "SecondPassHelper.h"
#include "Opcodes.h"

void secondPass(AssemblerState* state);
void processLineSecondPass(AssemblerState* state);
void processInstructionLine(AssemblerState* state, char* command, char* operands);

#endif