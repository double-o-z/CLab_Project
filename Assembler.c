// Assembler.c
#include "FileParser.h"
#include "MacroProcessor.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "CreateOutput.h"
#include "Assembler.h"
#include "stdio.h"

void assemble(const char* inputFilename) {
    printf("Assembling input file: %s\n", inputFilename);
    ParsedFile parsedFile = ParseFile(inputFilename);
    printf("Received %d parsed lines from ParseFile\n", parsedFile.numberOfLines);
    ProcessMacro(inputFilename);
    FirstPass(inputFilename);
    SecondPass(inputFilename);
    CreateOutput(inputFilename);
}
