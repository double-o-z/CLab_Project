#include "SecondPass.h"
#include <stdio.h>

void SecondPass(ParsedFile* parsedFile, AssemblerState* state) {
    printf("\n\nSecond pass for: %s\n", parsedFile->fileName);

    if (state->duplicateSymbols) {
        printf("Duplicate symbols found in this file, aborting second pass.\n");
        return;
    }

    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        // Process each line with the newly created helper function
        processLineSecondPass(state, parsedFile->lines[i], i + 1);
    }

    printf("Second pass completed for: %s\n", parsedFile->fileName);
}

void processInstructionLine(AssemblerState* state, char* line, int lineNumber) {
    printf("Processing instruction line second pass.\n");

}

void processLineSecondPass(AssemblerState* state, char* line, int lineNumber) {
    printf("Handling line: %s\n", line);
    char** parts = splitFirstWhitespace(line);
    char* label = NULL;
    char* command = parts[0];
    char* operands = parts[1];

    // Check if the line defines a label
    int len = strlen(command);
    if (command[len - 1] == ':') {
        command[len - 1] = '\0';  // Remove colon
        label = command;
        if (operands) {
            parts = splitFirstWhitespace(operands);
            command = parts[0];
            operands = parts[1];
        }
    }

    printf("handling command %s, operands: %s, and label: %s\n", command, operands, label);
    if (isDirective(line)) {
        // Skip directive lines
        printf("Skipping directive: %s\n", line);
    } else {
        // Process instruction lines
        processInstructionLine(state, line, lineNumber);
    }
}