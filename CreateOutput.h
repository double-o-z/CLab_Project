// CreateOutput.h
#ifndef CREATEOUTPUT_H
#define CREATEOUTPUT_H
#include "Helper.h"
#include "FirstPassHelper.h"

void CreateOutput(AssemblerState * state);
void createExtFile(AssemblerState* state);
void createEntFile(AssemblerState* state);
void createObjectFile(AssemblerState* state);
void encodeBase4(const char* binary, char* output);

#endif

