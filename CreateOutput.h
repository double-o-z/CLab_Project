// CreateOutput.h
#ifndef CREATEOUTPUT_H
#define CREATEOUTPUT_H
#include "Helper.h"
#include "FirstPassHelper.h"

void CreateOutput(AssemblerState * state, const char* fileName);
void createExtFile(AssemblerState* state, const char* fileName);
void createEntFile(AssemblerState* state, const char* fileName);
void createObjectFile(AssemblerState* state, const char* fileName);
void encodeBase4(const char* binary, char* output);

#endif

