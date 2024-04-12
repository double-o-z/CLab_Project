#include "MacroProcessor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 256

void ProcessMacro(ParsedFile* parsedFile) {
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
            char* macroNameEnd = strstr(macroNameStart, "\n");
            if (macroNameEnd == NULL) {
                macroNameEnd = macroNameStart + strlen(macroNameStart);
            }
            int macroNameLength = macroNameEnd - macroNameStart;

            MacroDefinition newMacro;
            newMacro.macroName = malloc(macroNameLength + 1);
            strncpy(newMacro.macroName, macroNameStart, macroNameLength);
            newMacro.macroName[macroNameLength] = '\0';
            newMacro.macroLines = malloc(sizeof(char*) * MAX_LINE_LENGTH);
            newMacro.lineCount = 0;

            // Copy macro lines until "endmcr"
            i++; // Move to next line after macro name
            while (i < parsedFile->numberOfLines && strstr(parsedFile->lines[i], "endmcr") != parsedFile->lines[i]) {
                newMacro.macroLines[newMacro.lineCount] = strdup(parsedFile->lines[i]);
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
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        char* line = parsedFile->lines[i];
        for (int j = 0; j < macroCount; j++) {
            char* macroName = macroDefinitions[j].macroName;
            if (strstr(line, macroName) != NULL) {
                // Replace macro call with macro lines
                for (int k = 0; k < macroDefinitions[j].lineCount; k++) {
                    char* macroLine = macroDefinitions[j].macroLines[k];
                    parsedFile->lines = realloc(parsedFile->lines, sizeof(char*) * (parsedFile->numberOfLines + 1));
                    memmove(&parsedFile->lines[i + 1], &parsedFile->lines[i], sizeof(char*) * (parsedFile->numberOfLines - i));
                    parsedFile->lines[i] = macroLine;
                    parsedFile->numberOfLines++;
                    i++;
                }
            }
        }
    }

    // Free memory allocated for macro definitions
    for (int i = 0; i < macroCount; i++) {
        free(macroDefinitions[i].macroName);
        for (int j = 0; j < macroDefinitions[i].lineCount; j++) {
            free(macroDefinitions[i].macroLines[j]);
        }
        free(macroDefinitions[i].macroLines);
    }
    free(macroDefinitions);
}
