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

void writeFileIfMacrosExist(ParsedFile* parsedFile) {
    if (parsedFile->numberOfLines > 0) {

        char* newFilename = malloc(strlen(parsedFile->fileName) + 3); // Extra space for null terminator and extension change
        strcpy(newFilename, parsedFile->fileName);

        // Find the last occurrence of '.as'
        char* extension = strrchr(newFilename, '.');
        if (extension != NULL && strcmp(extension, ".as") == 0) {
            strcpy(extension, ".am"); // Replace ".as" with ".am"
        } else {
            // If no ".as" found, append ".am"
            strcat(newFilename, ".am");
        }

        FILE* file = fopen(newFilename, "w");
        if (file != NULL) {
            for (int i = 0; i < parsedFile->numberOfLines; i++) {
                fprintf(file, "%s\n", parsedFile->lines[i]);
            }
            fclose(file);
            printf("Output file created: %s\n", newFilename);
        } else {
            printf("Failed to create output file.\n");
        }
        free(newFilename);
    } else {
        printf("No macros were processed; no output file created.\n");
    }
}

void ProcessMacro(ParsedFile* parsedFile) {
    printf("Processing macros for file: %s\n", parsedFile->fileName);

    MacroList macroList = { .macros = malloc(sizeof(Macro) * INITIAL_MACRO_LIST_CAPACITY), .capacity = INITIAL_MACRO_LIST_CAPACITY, .count = 0 };
    char** newLines = malloc(sizeof(char*) * parsedFile->numberOfLines);
    int newLineCount = 0;
    char macroName[MACRO_NAME_BUFFER_SIZE];
    int inMacro = 0;
    int macrosExist = 0;

    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        char* line = parsedFile->lines[i];

        if (sscanf(line, "mcr %99s", macroName) == 1) {
            inMacro = 1;
            macrosExist = 1;
            ensureMacroListCapacity(&macroList);
            Macro* newMacro = &macroList.macros[macroList.count++];
            newMacro->name = strDuplicate(macroName);
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
            macroList.macros[macroList.count - 1].lines[macroList.macros[macroList.count - 1].lineCount++] = strDuplicate(line);
            continue;
        }

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

        newLines[newLineCount++] = strDuplicate(line);
    }

    // Replace old lines with new lines
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        free(parsedFile->lines[i]);
    }
    free(parsedFile->lines);

    parsedFile->lines = newLines;
    parsedFile->numberOfLines = newLineCount;

    // Write to file if macros were processed
    if (macrosExist) {
        writeFileIfMacrosExist(parsedFile);
    } else {
        printf("No macros were processed; no changes were made to the file.\n");
    }

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