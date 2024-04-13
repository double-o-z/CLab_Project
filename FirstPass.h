// FirstPass.h
#ifndef FIRSTPASS_H
#define FIRSTPASS_H
#include "Helper.h"
#include "FirstPassHelper.h"

void FirstPass(ParsedFile* parsedFile);
void processLine(AssemblerState* state, char* line, int lineNumber);
void handleDefineDirective(AssemblerState* state, char* operands, int lineNumber, const char* fullLine);
void handleDataDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
void handleStringDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label);
void handleExternalDirective(AssemblerState* state, char* operands, int lineNumber, const char* line);

#endif

