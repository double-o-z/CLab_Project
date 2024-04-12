// Assembler.c
#include "FileParser.h"
#include "MacroProcessor.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "CreateOutput.h"
#include "Assembler.h"
#include "stdio.h"

void Assemble(const char* inputFilename) {
    printf("Assembling input file: %s\n", inputFilename);
    ParsedFile parsedFile = ParseFile(inputFilename);
    ProcessMacro(&parsedFile);
    FirstPass(&parsedFile);
    SecondPass(&parsedFile);
    CreateOutput(&parsedFile);
}
