#include "MacroProcessor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void ensureMacroCapacity(Macro* macro) {
    if (macro->lineCount >= macro->capacity) {
        macro->capacity += INCREMENTAL_MACRO_CAPACITY;
        macro->lines = realloc(macro->lines, sizeof(char*) * macro->capacity);
        if (macro->lines == NULL) {
            fprintf(stderr, "Memory allocation failed for macro lines!\n");
            exit(EXIT_FAILURE);
        }
    }
}

void ensureMacroListCapacity(MacroList* list) {
    if (list->count >= list->capacity) {
        list->capacity += INCREMENTAL_MACRO_LIST_CAPACITY;
        list->macros = realloc(list->macros, sizeof(Macro) * list->capacity);
        if (list->macros == NULL) {
            fprintf(stderr, "Memory allocation failed for macros list!\n");
            exit(EXIT_FAILURE);
        }
    }
}

void writeFileIfMacrosExist(AssemblerState* state) {
    FILE* file;
    char* fullFilename;

    if (state->parsedFile.numberOfLines > 0) {
        fullFilename = malloc(strlen(state->inputFilename) + 4); /*  +3 for ".as" +1 for null terminator */
        sprintf(fullFilename, "%s.am", state->inputFilename);

        file = fopen(fullFilename, "w");
        if (file != NULL) {
            int i;
            for (i = 0; i < state->parsedFile.numberOfLines; i++) {
                fprintf(file, "%s\n", state->parsedFile.lines[i]);
            }
            fclose(file);
            printf("Output file created: %s\n", fullFilename);
        } else {
            printf("Failed to create output file: %s\n", fullFilename);
        }
        free(fullFilename);
    } else {
        printf("No macros were processed; no output file created.\n");
    }
}

void processMacro(AssemblerState* state) {
    MacroList macroList;
    Macro *newMacro;
    int i, j, newLineCount, inMacro, found, k, macrosExist;
    char **newLines, macroName[MACRO_NAME_BUFFER_SIZE], *line;

    if (state->debugMode){
        printf("Processing macros for file: %s\n", state->inputFilename);
    }

    macroList.macros = malloc(sizeof(Macro) * INITIAL_MACRO_LIST_CAPACITY);
    macroList.capacity = INITIAL_MACRO_LIST_CAPACITY;
    macroList.count = 0;
    newLines = malloc(sizeof(char*) * state->parsedFile.numberOfLines);
    newLineCount = 0;
    inMacro = 0;
    macrosExist = 0;
    for (i = 0; i < state->parsedFile.numberOfLines; i++) {
        line = state->parsedFile.lines[i];

        if (sscanf(line, "mcr %99s", macroName) == 1) {
            inMacro = 1;
            macrosExist = 1;
            ensureMacroListCapacity(&macroList);
            newMacro = &macroList.macros[macroList.count++];
            newMacro->name = strdup(macroName);
            newMacro->capacity = INITIAL_MACRO_CAPACITY;
            newMacro->lines = malloc(sizeof(char*) * newMacro->capacity);
            newMacro->lineCount = 0;
            continue;
        }

        if (strstr(line, "endmcr")) {
            inMacro = 0;
            continue;
        }

        if (inMacro) {
            ensureMacroCapacity(&macroList.macros[macroList.count - 1]);
            macroList.macros[macroList.count - 1]
            .lines[macroList.macros[macroList.count - 1].lineCount++] = strdup(line);
            continue;
        }

        if (sscanf(line, "%99s", macroName) == 1 && !inMacro) {
            found = 0;
            for (j = 0; j < macroList.count; j++) {
                if (strcmp(macroList.macros[j].name, macroName) == 0) {
                    for (k = 0; k < macroList.macros[j].lineCount; k++) {
                        newLines[newLineCount++] = strdup(macroList.macros[j].lines[k]);
                    }
                    found = 1;
                    break;
                }
            }
            if (found) continue;
        }

        newLines[newLineCount++] = strdup(line);
    }

    /*  Replace old lines with new lines */
    for (i = 0; i < state->parsedFile.numberOfLines; i++) {
        free(state->parsedFile.lines[i]);
    }
    free(state->parsedFile.lines);

    state->parsedFile.lines = newLines;
    state->parsedFile.numberOfLines = newLineCount;

    /*  Write to file if macros were processed */
    if (macrosExist) {
        writeFileIfMacrosExist(state);
        printf("Found and processed macros; From now on, errors are reported with %s.am file.\n",
               state->inputFilename);
    } else {
        printf("No macros were processed; no changes were made to the file.\n");
    }

    /*  Cleanup */

    for (i = 0; i < macroList.count; i++) {
        for (j = 0; j < macroList.macros[i].lineCount; j++) {
            free(macroList.macros[i].lines[j]);
        }
        free(macroList.macros[i].lines);
        free(macroList.macros[i].name);
    }

    free(macroList.macros);
    if (state->debugMode){
        printAllLines(state);
    }
}