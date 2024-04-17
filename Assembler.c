// Assembler.c
#include "Helper.h"
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
    AssemblerState state = initAssemblerState();
    ProcessMacro(&parsedFile); // TODO: trim all whitespace and tabs after process macro, for file.am?
    FirstPass(&parsedFile, &state);
    SecondPass(&parsedFile, &state);
    CreateOutput(&parsedFile);
    printf("\n\nFinished Assembling input file: %s\n\n\n", inputFilename);
}
