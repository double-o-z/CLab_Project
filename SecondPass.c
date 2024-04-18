#include "SecondPass.h"
#include <stdio.h>

void SecondPass(ParsedFile* parsedFile, AssemblerState* state) {
    printf("\n\nSecond pass for: %s\n", parsedFile->fileName);

    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        // Process each line with the newly created helper function
        processLineSecondPass(state, parsedFile->lines[i], i + 1);
    }

    // Call helper functions to print symbols table and data list
    printDataList(state);
    printSymbolsTable(state);
    printExternalsTable(state);
    printInstructionsList(state);
    printf("\n\nSecond pass completed for: %s\n", parsedFile->fileName);
}

void processInstructionLine(AssemblerState* state, char* command, char* operands, int lineNumber) {
    // Find opcode index based on the command
    int opcodeIndex = findOpcodeIndex(command);
    if (opcodeIndex == -1) {
        printf("Error on line %d: Unknown command '%s'\n", lineNumber, command);
        return;
    }

    // Validate and parse operands
    int srcType = -1, destType = -1;
    if (!parseOperands(state, &srcType, &destType, operands, opcodes[opcodeIndex])) {
        printf("Error on line %d: Invalid operands for '%s'\n", lineNumber, command);
        return;
    }

    // Move to the next instruction location before encoding additional words
    state->instructionCounter++;

    // Encode the additional operand data words
    encodeOperandDataWords(state, srcType, destType, operands);
}

void processLineSecondPass(AssemblerState* state, char* line, int lineNumber) {
    printf("\nHandling with instructionCount: %d, with statement: %s\n",
           state->instructionCounter + INDEX_FIRST_INSTRUCTION, line);
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

    printf("handling command %s, operands: %s, and label: %s. "
           "First word value: %d\n", command, operands, label, state->instructions.array[state->instructionCounter]);
    if (strcmp(command, ".entry") == 0) {
        Symbol* symbol = findSymbolInST(state, operands);
        if (symbol != NULL && (symbol->type == DATA || symbol->type == CODE)) {
            symbol->type = ENTRY;
            state->entriesExist = true;
        } else {
            fprintf(stderr, "Error: .entry directive for non-DATA "
                            "type symbol or undefined symbol in file at line %d\n", lineNumber);
        }
    } else if (isDirective(line)) {
        printf("Skipping directive: %s\n", line);
    } else {
        processInstructionLine(state, command, operands, lineNumber);
    }

    // Clean up parts
    free(parts[0]);
    if (parts[1] != NULL) {
        free(parts[1]);
    }
    free(parts);
}