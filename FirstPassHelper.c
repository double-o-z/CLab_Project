#include "FirstPassHelper.h"

// Initialize assembler state
AssemblerState initAssemblerState() {
    AssemblerState state;
    state.instructions = calloc(4096, sizeof(int));  // 4096 words for instructions
    state.instructionCount = 0;
    state.data = malloc(5 * sizeof(int));            // Dynamic data list
    state.dataCapacity = 5;
    state.dataCount = 0;
    state.symbols = malloc(10 * sizeof(Symbol));     // Dynamic symbols table
    state.symbolsCapacity = 10;
    state.symbolsCount = 0;
    return state;
}

// Function to split a string by the first whitespace
char** splitFirstWhitespace(char* str) {
    char** parts = malloc(2 * sizeof(char*));
    char* firstSpace = strchr(str, ' ');
    if (firstSpace != NULL) {
        *firstSpace = '\0';  // Split the string
        parts[0] = str;
        parts[1] = firstSpace + 1;
    } else {
        parts[0] = str;
        parts[1] = NULL;
    }
    return parts;
}

// Helper function to trim whitespaces and tabs
char* trim(char* str) {
    char* end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';
    return str;
}

// Process a line to determine its type and handle labels
void processLine(AssemblerState* state, char* line, int lineNumber) {
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

    // Handle command type determination here (to be implemented)
    // For example, determine if it's a data directive or an instruction
    // Handle .define directive
    if (strcmp(command, ".define") == 0) {
        handleDefineDirective(state, operands, lineNumber, line);
    } else if (strcmp(command, ".data") == 0) {
        handleDataDirective(state, operands, lineNumber, line, label);
    } else if (strcmp(command, ".string") == 0) {
        handleStringDirective(state, operands, lineNumber, line, label);
    }

    free(parts);  // Free parts array after use
}

// Function to handle the .define directive
void handleDefineDirective(AssemblerState* state, char* operands, int lineNumber, const char* fullLine) {
    if (!operands) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }

    // Trim and split by '='
    operands = trim(operands);
    char* equalSign = strchr(operands, '=');
    if (!equalSign) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }
    *equalSign = '\0';
    char* name = trim(operands);
    char* valueStr = trim(equalSign + 1);

    // Validate name and value
    if (!isalpha(name[0]) || !isdigit(valueStr[0])) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        return;
    }

    // Convert value to integer
    int value = atoi(valueStr);

    // Add to symbols table
    ensureCapacity((void**)&state->symbols, state->symbolsCount, state->symbolsCapacity, sizeof(Symbol));
    Symbol newSymbol = {strdup(name), MDEFINE, value};
    state->symbols[state->symbolsCount++] = newSymbol;
}

// Helper function to check if a string is a valid integer
int isValidInteger(const char* str) {
    if (*str == '+' || *str == '-')  // Skip the sign if present
        str++;
    if (*str == '\0')  // String is only a sign
        return 0;
    while (*str) {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

// Helper function to find a symbol in the symbol table and return its value
int findSymbolValue(const AssemblerState* state, const char* label) {
    for (int i = 0; i < state->symbolsCount; i++) {
        if (strcmp(state->symbols[i].label, label) == 0 && state->symbols[i].type == MDEFINE) {
            return state->symbols[i].value;
        }
    }
    return -1;  // Return -1 if not found, you can handle this case based on your error handling strategy
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

    // Split the operands by commas to extract individual values
    char* token = strtok(operands, ",");
    int index = state->dataCount;  // Start index in data array for this label

    while (token) {
        token = trim(token);  // Trim whitespace around the number or label

        int value;
        if (isValidInteger(token)) {
            value = atoi(token);  // Convert token to integer if it's a valid number
        } else {
            value = findSymbolValue(state, token);  // Look up the symbol in the symbol table
            if (value == -1) {  // Handle undefined symbols or invalid integers
                fprintf(stderr, "Error in line: %d, invalid operand or undefined symbol used in .data: %s\n", lineNumber, token);
                token = strtok(NULL, ",");  // Continue to next token despite error
                continue;
            }
        }

        // Ensure the data array has enough capacity
        ensureCapacity((void**)&state->data, state->dataCount, state->dataCapacity, sizeof(int));
        state->data[state->dataCount++] = value;  // Store the integer or resolved symbol value in the data array

        token = strtok(NULL, ",");  // Move to the next token
    }

    // Add to symbols table
    ensureCapacity((void**)&state->symbols, state->symbolsCount, state->symbolsCapacity, sizeof(Symbol));
    Symbol newSymbol = {strdup(label), DATA, index};
    state->symbols[state->symbolsCount++] = newSymbol;
}

// Function to print the symbols table
void printSymbolsTable(const AssemblerState* state) {
    printf("\nSymbols Table:\n");
    printf("Label\tType\tValue\n");
    for (int i = 0; i < state->symbolsCount; i++) {
        char* type;
        switch (state->symbols[i].type) {
            case MDEFINE: type = "MDEFINE"; break;
            case CODE: type = "CODE"; break;
            case DATA: type = "DATA"; break;
            default: type = "UNKNOWN"; break;
        }
        printf("%s\t%s\t%d\n", state->symbols[i].label, type, state->symbols[i].value);
    }
}

// Function to print the data list
void printDataList(const AssemblerState* state) {
    printf("\nData List:\n");
    for (int i = 0; i < state->dataCount; i++) {
        printf("%d: %d\n", i, state->data[i]);
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

    // Remove the enclosing double quotes
    operands[strlen(operands) - 1] = '\0';
    operands++;  // Move past the initial double quote

    // Check that all characters are ASCII and copy each character's ASCII value to the data list
    char* character = operands;
    int index = state->dataCount;  // Start index in data array for this label

    while (*character) {
        if ((unsigned char)*character > 127) {
            fprintf(stderr, "Error in line: %d, non-ASCII character found in .string: %s\n", lineNumber, line);
            return;
        }
        ensureCapacity((void**)&state->data, state->dataCount, state->dataCapacity, sizeof(int));
        state->data[state->dataCount++] = (int)*character;  // Store the ASCII value of the character
        character++;
    }

    // Add the terminating zero to follow the C string convention in the data list
    ensureCapacity((void**)&state->data, state->dataCount, state->dataCapacity, sizeof(int));
    state->data[state->dataCount++] = 0;

    // Add to symbols table
    ensureCapacity((void**)&state->symbols, state->symbolsCount, state->symbolsCapacity, sizeof(Symbol));
    Symbol newSymbol = {strdup(label), DATA, index};
    state->symbols[state->symbolsCount++] = newSymbol;
}