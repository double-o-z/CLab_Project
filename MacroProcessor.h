// MacroProcessor.h
#ifndef MACROPROCESSOR_H
#define MACROPROCESSOR_H
#include "Helper.h"

#define INITIAL_MACRO_CAPACITY 5
#define INCREMENTAL_MACRO_CAPACITY 5
#define INITIAL_MACRO_LIST_CAPACITY 5
#define INCREMENTAL_MACRO_LIST_CAPACITY 5
#define MACRO_NAME_BUFFER_SIZE 100

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

