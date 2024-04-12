#include "MacroProcessor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 256

void ensureMacroCapacity(Macro* macro) {
    if (macro->lineCount >= macro->capacity) {
        macro->capacity += 5;
        macro->lines = realloc(macro->lines, sizeof(char*) * macro->capacity);
        if (macro->lines == NULL) {
            fprintf(stderr, "Memory allocation failed for macro lines!\n");
            exit(1);
        }
    }
}

void ensureMacroListCapacity(MacroList* list) {
    if (list->count >= list->capacity) {
        list->capacity += 5;
        list->macros = realloc(list->macros, sizeof(Macro) * list->capacity);
        if (list->macros == NULL) {
            fprintf(stderr, "Memory allocation failed for macros list!\n");
            exit(1);
        }
    }
}

void ProcessMacro(ParsedFile* parsedFile) {
    MacroList macroList = { .macros = malloc(sizeof(Macro) * 5), .capacity = 5, .count = 0 };
    char** newLines = malloc(sizeof(char*) * parsedFile->numberOfLines);
    int newLineCount = 0;
    char macroName[100];
    int inMacro = 0;

    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        char* line = parsedFile->lines[i];

        // Handling macro definitions
        if (sscanf(line, "mcr %99s", macroName) == 1) {
            inMacro = 1;
            ensureMacroListCapacity(&macroList);
            Macro* newMacro = &macroList.macros[macroList.count++];
            newMacro->name = strDuplicate(macroName);
            newMacro->capacity = 5;
            newMacro->lines = malloc(sizeof(char*) * newMacro->capacity);
            newMacro->lineCount = 0;
            continue;
        }

        if (strstr(line, "endmcr") && inMacro) {
            inMacro = 0;
            continue;
        }

        if (inMacro) {
            ensureMacroCapacity(&macroList.macros[macroList.count - 1]);
            macroList.macros[macroList.count - 1].lines[macroList.macros[macroList.count - 1].lineCount++] = strDuplicate(line);
            continue;
        }

        // Expanding macros
        if (sscanf(line, "%99s", macroName) == 1 && !inMacro) {
            int found = 0;
            for (int j = 0; j < macroList.count; j++) {
                if (strcmp(macroList.macros[j].name, macroName) == 0) {
                    for (int k = 0; k < macroList.macros[j].lineCount; k++) {
                        newLines[newLineCount++] = strDuplicate(macroList.macros[j].lines[k]);
                    }
                    found = 1;
                    break;
                }
            }
            if (found) continue;
        }

        // Regular lines
        newLines[newLineCount++] = strDuplicate(line);
    }

    // Replace old lines with new lines
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        free(parsedFile->lines[i]);
    }
    free(parsedFile->lines);

    parsedFile->lines = newLines;
    parsedFile->numberOfLines = newLineCount;

    // Cleanup
    for (int i = 0; i < macroList.count; i++) {
        for (int j = 0; j < macroList.macros[i].lineCount; j++) {
            free(macroList.macros[i].lines[j]);
        }
        free(macroList.macros[i].lines);
        free(macroList.macros[i].name);
    }
    free(macroList.macros);

    printAllLines(*parsedFile);
}