// Assembler.c
#include "FileParser.h"
#include "MacroProcessor.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "CreateOutput.h"
#include "Assembler.h"

void assemble(const char* inputFilename) {
    ParseFile(inputFilename);
    ProcessMacro(inputFilename);
    FirstPass(inputFilename);
    SecondPass(inputFilename);
    CreateOutput(inputFilename);
}
