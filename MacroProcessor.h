// MacroProcessor.h
#ifndef MACROPROCESSOR_H
#define MACROPROCESSOR_H
#include "helper.h"

typedef struct {
    char* name;
    char** lines;
    int lineCount;
    int capacity;
} Macro;

typedef struct {
    Macro* macros;
    int count;
    int capacity;
} MacroList;

void ensureMacroCapacity(Macro* macro);
void ensureMacroListCapacity(MacroList* list);

void ProcessMacro(ParsedFile* parsedFile);

#endif

