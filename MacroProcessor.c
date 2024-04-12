#include "MacroProcessor.h"
#include "helper.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 256

void ProcessMacro(ParsedFile* parsedFile) {
    printf("Processing macros for file: %s\n", parsedFile->fileName);

    // Dictionary to store macro definitions
    // Key: MACRO_NAME, Value: List of macro lines
    typedef struct {
        char* macroName;
        char** macroLines;
        int lineCount;
    } MacroDefinition;

    MacroDefinition* macroDefinitions = NULL;
    int macroCount = 0;

    // Iterate over the lines to find macro definitions
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        char* line = parsedFile->lines[i];
        if (strstr(line, "mcr ") == line) { // Found a macro definition
            char* macroNameStart = line + 4; // Skip "mcr "
            char* macroNameEnd = strchr(macroNameStart, '\n');
            if (macroNameEnd == NULL) {
                macroNameEnd = macroNameStart + strlen(macroNameStart);
            }

            MacroDefinition newMacro;
            newMacro.macroName = strDuplicate(macroNameStart);
            newMacro.macroLines = malloc(sizeof(char*) * MAX_LINE_LENGTH);
            newMacro.lineCount = 0;

            // Copy macro lines until "endmcr"
            i++; // Move to next line after macro name
            while (i < parsedFile->numberOfLines && strstr(parsedFile->lines[i], "endmcr") != parsedFile->lines[i]) {
                newMacro.macroLines[newMacro.lineCount] = strDuplicate(parsedFile->lines[i]);
                newMacro.lineCount++;
                i++;
            }

            // Add new macro definition to dictionary
            macroCount++;
            macroDefinitions = realloc(macroDefinitions, sizeof(MacroDefinition) * macroCount);
            macroDefinitions[macroCount - 1] = newMacro;
        }
    }

    // Replace macro calls with macro lines
    char** updatedLines = malloc(sizeof(char*) * parsedFile->numberOfLines * MAX_LINE_LENGTH);
    int updatedLinesCount = 0;
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        char* line = parsedFile->lines[i];
        int macroFound = 0;
        for (int j = 0; j < macroCount; j++) {
            char* macroName = macroDefinitions[j].macroName;
            char* macroCall = strstr(line, macroName);
            if (macroCall != NULL) {
                // Replace macro call with macro lines
                for (int k = 0; k < macroDefinitions[j].lineCount; k++) {
                    updatedLines[updatedLinesCount] = strDuplicate(macroDefinitions[j].macroLines[k]);
                    updatedLinesCount++;
                }
                macroFound = 1;
                break;
            }
        }
        if (!macroFound) {
            // Copy original line if no macro call was found
            updatedLines[updatedLinesCount] = strDuplicate(line);
            updatedLinesCount++;
        }
        // Free the original line after processing it
        free(parsedFile->lines[i]);
    }

    // Free memory allocated for the original lines array
    free(parsedFile->lines);

    // Update parsedFile with the updated lines
    parsedFile->lines = updatedLines;
    parsedFile->numberOfLines = updatedLinesCount;

    // Free memory allocated for macro definitions
    for (int i = 0; i < macroCount; i++) {
        free(macroDefinitions[i].macroName);
        for (int j = 0; j < macroDefinitions[i].lineCount; j++) {
            free(macroDefinitions[i].macroLines[j]);
        }
        free(macroDefinitions[i].macroLines);
    }
    free(macroDefinitions);

    printAllLines(*parsedFile);
}