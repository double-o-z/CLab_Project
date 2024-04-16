#include "FirstPass.h"

// Main function to start the first pass
void FirstPass(ParsedFile* parsedFile, AssemblerState* state) {
    printf("First pass for: %s\n", parsedFile->fileName);

    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        // Pass the current line number, i, to processLine; line numbers start at 1 for user clarity
        processLine(state, parsedFile->lines[i], i + 1);
    }

    // Call helper functions to print symbols table and data list
    printSymbolsTable(state);
    printDataList(state);
    printInstructionsList(state);
}

// Process a line to determine its type and handle labels
void processLine(AssemblerState* state, char* line, int lineNumber) {
    printf("%s\n", line);
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

    printf("handling command %s, operands: %s\n", command, operands);
    // Handle command type determination here (to be implemented)
    // For example, determine if it's a data directive or an instruction
    // Handle .define directive
    if (strcmp(command, ".define") == 0) {
        handleDefineDirective(state, operands, lineNumber, line);
    } else if (strcmp(command, ".data") == 0) {
        handleDataDirective(state, operands, lineNumber, line, label);
    } else if (strcmp(command, ".string") == 0) {
        handleStringDirective(state, operands, lineNumber, line, label);
    } else if (strcmp(command, ".extern") == 0) {
        handleExternalDirective(state, operands, lineNumber, line);
    } else {
        // If it's not a directive, treat it as an instruction
        handleInstructions(state, label, command, operands, lineNumber);
    }

    free(parts);  // Free parts array after use
}

void handleInstructions(AssemblerState* state, char* label, char* command, char* operands, int lineNumber) {
    // If a label is present, add it to the symbol table with the current instruction count (IC)
    if (label) {
        Symbol newSymbol = {strdup(label), CODE, state->instructions.count};
        dynamicInsertSymbol(state, newSymbol);
    }
    // Find opcode index based on the command
    int opcodeIndex = findOpcodeIndex(command);  // Assuming this function exists
    if (opcodeIndex == -1) {
        printf("Error on line %d: Unknown command '%s'\n", lineNumber, command);
        return;
    }
    // Validate and parse operands
    int srcType = -1, destType = -1;
    if (!parseOperands(&srcType, &destType, operands, opcodes[opcodeIndex])) {
        printf("Error on line %d: Invalid operands for '%s'\n", lineNumber, command);
        return;
    }
    // Create the first word of the instruction
    int firstWord = createFirstWord(srcType, destType, opcodes[opcodeIndex].opcode);
    printf("adding first instruction word with value: %d\n", firstWord);
    addInstructionToInstructionList(&state->instructions, firstWord);

    // Prepare for additional words that describe operand data (handled during the second pass)
    prepareOperandDataWords(state, srcType, destType);
}

// Function to handle the .define directive
void handleDefineDirective(AssemblerState* state, char* operands, int lineNumber, const char* fullLine) {
    if (!operands) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }
    operands = trim(operands);
    char* equalSign = strchr(operands, '=');
    if (!equalSign) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }
    *equalSign = '\0';
    char* name = trim(operands);
    char* valueStr = trim(equalSign + 1);

    if (!isalpha(name[0]) || !isValidInteger(valueStr)) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }

    int value = atoi(valueStr);
    Symbol newSymbol = {strdup(name), MDEFINE, value};
    dynamicInsertSymbol(state, newSymbol);
}

// Function to handle the .data directive
void handleDataDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label) {
    if (!label || label[0] == '\0') {
        fprintf(stderr, "Error in line: %d, .data command requires a label: %s\n", lineNumber, line);
        return;
    }

    if (!operands) {
        fprintf(stderr, "Error in line: %d, invalid .data command, no data provided: %s\n", lineNumber, line);
        return;
    }

    int countBeforeInsert = state->data.count;
    char* token = strtok(operands, ",");
    while (token) {
        token = trim(token);
        int value;
        if (isValidInteger(token)) {
            value = atoi(token); // Convert to int
        } else {
            value = findSymbolValue(state, token);
            if (value == -1) {
                fprintf(stderr, "Error in line: %d, invalid operand or undefined symbol used in .data: %s\n", lineNumber, token);
                token = strtok(NULL, ",");
                continue;
            }
        }
        dynamicInsert(&state->data, value);
        token = strtok(NULL, ",");
    }

    Symbol newSymbol = {strdup(label), DATA, countBeforeInsert};
    dynamicInsertSymbol(state, newSymbol);
}

// Function to handle the .data directive
void handleExternalDirective(AssemblerState* state, char* operands, int lineNumber, const char* line) {
    if (!operands) {
        fprintf(stderr, "Error in line: %d, invalid .extern command, no data provided: %s\n", lineNumber, line);
        return;
    }

    char* token = strtok(operands, ",");
    while (token) {
        token = trim(token);
        int value;
        if (isValidInteger(token)) {
            value = atoi(token); // Convert to int
        } else {
            value = findSymbolValue(state, token);
            if (value != -1) {
                fprintf(stderr, "Error in line: %d, invalid operand for .extern command, "
                                "it is already defined: %s\n", lineNumber, token);
                token = strtok(NULL, ",");
                continue;
            }
        }
        Symbol newSymbol = {strdup(token), EXTERNAL, 0};
        dynamicInsertSymbol(state, newSymbol);
        token = strtok(NULL, ",");
    }
}

// Function to handle the .string directive
void handleStringDirective(AssemblerState* state, char* operands, int lineNumber, const char* line, const char* label) {
    if (!label || label[0] == '\0') {
        fprintf(stderr, "Error in line: %d, .string command requires a label: %s\n", lineNumber, line);
        return;
    }

    if (!operands || operands[0] != '"' || operands[strlen(operands) - 1] != '"') {
        fprintf(stderr, "Error in line: %d, invalid .string command, string must be enclosed in double quotes: %s\n", lineNumber, line);
        return;
    }
    int countBeforeInsert = state->data.count;
    operands++;  // Skip the initial double quote
    operands[strlen(operands) - 1] = '\0'; // Remove the closing double quote
    for (char* character = operands; *character; character++) {
        if ((unsigned char)*character > 127) {
            fprintf(stderr, "Error in line: %d, non-ASCII character found in .string: %s\n", lineNumber, line);
            return;
        }
        dynamicInsert(&state->data, (int)*character);
    }

    dynamicInsert(&state->data, 0); // Add terminating zero
    Symbol newSymbol = {strdup(label), DATA, countBeforeInsert};
    dynamicInsertSymbol(state, newSymbol);
}