// Assembler.h
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "Helper.h"

void Assemble(const char* inputFilename);
void freeParsedFile(ParsedFile* parsedFile);
void freeAssemblerState(AssemblerState* state);

#endif

