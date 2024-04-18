#include "SecondPass.h"
#include <stdio.h>

void SecondPass(AssemblerState* state) {
    if (state->debugMode){
        printf("\n\n\nSecond pass for: %s\n", state->inputFilename);
    }

    for (state->parsedFile.currentLineNum = 0;
         state->parsedFile.currentLineNum < state->parsedFile.numberOfLines;
         state->parsedFile.currentLineNum++) {
        // Process each line with the newly created helper function
        processLineSecondPass(state);
    }

    // Call helper functions to print symbols table and data list
    if (state->debugMode){
        printDataList(state);
        printSymbolsTable(state);
        printExternalsTable(state);
        printInstructionsList(state);
        printf("\n\n\nSecond pass completed for: %s\n", state->inputFilename);
    }
}

void processInstructionLine(AssemblerState* state, char* command, char* operands) {
    // Find opcode index based on the command
    int opcodeIndex = findOpcodeIndex(command);
    if (opcodeIndex == -1) {
        printf("Error on line %d: Unknown command '%s'\n"
               "with line: %s\n",
               state->parsedFile.currentLineNum + 1,
               command,
               state->parsedFile.lines[state->parsedFile.currentLineNum]);
        state->assemblerError = true;
        return;
    }

    // Validate and parse operands
    int srcType = -1, destType = -1;
    if (!parseOperands(state, &srcType, &destType, operands, opcodes[opcodeIndex])) {
        printf("Error on line %d: Invalid operands for '%s'\n", state->parsedFile.currentLineNum + 1, command);
        state->assemblerError = true;
        return;
    }

    // Move to the next instruction location before encoding additional words.
    // First word of each instruction was calculated and inserted during first pass.
    state->instructionCounter++;

    // Encode the additional operand data words (between 0 and 4 additional words).
    encodeOperandDataWords(state, srcType, destType, operands);
}

void processLineSecondPass(AssemblerState* state) {
    char* line = state->parsedFile.lines[state->parsedFile.currentLineNum];
    int lineNumber = state->parsedFile.currentLineNum + 1;
    if (state->debugMode){
        printf("\nHandling line number: %d, with text: %s\n",
               state->parsedFile.currentLineNum + 1, line);
    }

    // Skip empty lines and lines starting with ';'
    if (line[0] == '\0' || line[0] == ';') {
        return;
    }

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

    if (state->debugMode){
        printf("Handling command %s, operands: %s, and label: %s. "
               "First word value: %d\n", command, operands, label, state->instructions.array[state->instructionCounter]);
    }

    if (strcmp(command, ".entry") == 0) { // Update symbol as entry in symbols table, now that it should exist there.
        Symbol* symbol = findSymbolInST(state, operands);
        if (symbol != NULL && (symbol->type == DATA || symbol->type == CODE)) {
            symbol->type = ENTRY;
            state->entriesExist = true;
        } else {
            if (symbol != NULL && symbol->type == ENTRY) {
                fprintf(stderr, "Error: .entry directive, "
                                "called multiple times for symbol %s in file, "
                                "at line %d\n", symbol->label, lineNumber);
            } else {
                fprintf(stderr, "Error: .entry directive for non-DATA "
                                "type symbol or undefined symbol in file at line %d\n", lineNumber);
            }

            state->assemblerError = true;
        }
    } else if (isDirective(command)) {
        if (state->debugMode){
            printf("Found directive, skipping: %s\n", line);
        }
    } else {
        processInstructionLine(state, command, operands);
    }

    // Clean up parts
    free(parts[0]);
    if (parts[1] != NULL) {
        free(parts[1]);
    }
    free(parts);
}