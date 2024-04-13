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
    } else {
        processInstruction(state, command, operands, lineNumber);
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
    Symbol newSymbol = {strdup(name), MDEFINE, value};
    addSymbolAndUpdateUnresolved(state, newSymbol);
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
        ensureCapacity((void**)&state->data, &state->dataCapacity, state->dataCount + 1, sizeof(int));
        state->data[state->dataCount++] = value;  // Store the integer or resolved symbol value in the data array

        token = strtok(NULL, ",");  // Move to the next token
    }

    // Add to symbols table
    Symbol newSymbol = {strdup(label), DATA, index};
    addSymbolAndUpdateUnresolved(state, newSymbol);
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
        ensureCapacity((void**)&state->data, &state->dataCapacity, state->dataCount + 1, sizeof(int));
        state->data[state->dataCount++] = (int)*character;  // Store the ASCII value of the character
        character++;
    }

    // Add the terminating zero to follow the C string convention in the data list
    ensureCapacity((void**)&state->data, &state->dataCapacity, state->dataCount + 1, sizeof(int));
    state->data[state->dataCount++] = 0;

    // Add to symbols table
    Symbol newSymbol = {strdup(label), DATA, index};
    addSymbolAndUpdateUnresolved(state, newSymbol);
}

//// Helper function to check if a string represents a valid integer
//int isValidInteger(const char* str) {
//    char* end;
//    long val = strtol(str, &end, 10);
//    return *end == '\0' && *str != '\0';
//}

// Function to extract an integer from a symbol or numeric literal
int getOperandValue(AssemblerState* state, const char* operand) {
    if (isdigit(operand[0]) || operand[0] == '-' || operand[0] == '+') {
        return atoi(operand);  // Directly convert numeric literals
    } else {
        // Search for the symbol in the symbol table
        for (int i = 0; i < state->symbolsCount; i++) {
            if (strcmp(state->symbols[i].label, operand) == 0) {
                return state->symbols[i].value;  // Return the value associated with the symbol
            }
        }
    }
    return -1;  // Return -1 if the symbol is not found or input is invalid
}

// Checks if the operand is a valid register and returns its numeric index if it is.
int resolveRegister(const char* operand) {
    if (strlen(operand) == 2 && operand[0] == 'r') {
        int reg = operand[1] - '0'; // Convert character to integer (e.g., '0' to 0)
        if (reg >= 0 && reg <= 7) {
            return reg;  // Returns 0 to 7 for 'r0' to 'r7'
        }
    }
    return -1;  // Not a valid register
}

// Resolves an operand and determines its type
int resolveOperand(AssemblerState* state, char* operand, int* resolvedValue) {
    // Immediate type operand
    if (operand[0] == '#') {
        if (isValidInteger(operand + 1)) {
            *resolvedValue = atoi(operand + 1);
            return 0; // Immediate type
        }
    }

    // Constant Index type operand
    char* base = strtok(operand, "[");
    char* indexStr = strtok(NULL, "]");
    if (indexStr) {
        int indexValue;
        if (isValidInteger(indexStr)) {
            indexValue = atoi(indexStr);  // Convert directly if it's a numeric literal
        } else {
            indexValue = getOperandValue(state, indexStr);  // Resolve the value if it's a symbol
            if (indexValue == -1) {
                fprintf(stderr, "Error: Undefined symbol for index '%s'\n", indexStr);
                return -1;  // Index symbol not found or invalid
            }
        }

        int baseValue = getOperandValue(state, base);
        if (baseValue != -1) {
            *resolvedValue = baseValue + indexValue;
            return 2; // Constant Index type
        } else {
            fprintf(stderr, "Error: Undefined base symbol '%s'\n", base);
            return -1;  // Base symbol not found
        }
    }

    // Register type operand
    int reg = resolveRegister(operand);
    if (reg != -1) {
        *resolvedValue = reg;  // Register index as the value
        return 3; // Register type
    }

    // Direct type operand
    int directValue = getOperandValue(state, operand);
    if (directValue != -1) {
        *resolvedValue = directValue;
        return 1; // Direct type
    }

    return -1; // Operand could not be resolved
}

// Helper function to insert words into the instruction list based on the opcode and operands
void insertWords(AssemblerState* state, Opcode* cmd, int srcType, int srcValue, int dstType, int dstValue) {
    // This is a simplified placeholder. You'll need to implement the logic based on your specific machine architecture.
    state->instructions[state->instructionCount++] = (cmd->opcode << 12) | (srcType << 8) | (dstType << 4); // Example encoding
    if (srcType != -1) {
        state->instructions[state->instructionCount++] = srcValue;
    }
    if (dstType != -1) {
        state->instructions[state->instructionCount++] = dstValue;
    }
}

// Function to find an opcode structure based on the command name
Opcode* findOpcode(const char* command) {
    for (int i = 0; i < OPCODE_COUNT; i++) {
        if (strcmp(opcodes[i].name, command) == 0) {
            return &opcodes[i]; // Return a pointer to the opcode structure
        }
    }
    return NULL; // Return NULL if no matching opcode is found
}

void addUnresolved(AssemblerState* state, int instructionIndex, int lineNumber, char* line) {
    if (state->unresolvedCount >= state->unresolvedCapacity) {
        state->unresolvedCapacity += 10;
        state->unresolved = realloc(state->unresolved, sizeof(UnresolvedInstruction) * state->unresolvedCapacity);
    }
    state->unresolved[state->unresolvedCount++] = (UnresolvedInstruction){instructionIndex, lineNumber, strdup(line)};
}

// Enhanced operand resolution and validation
void processInstruction(AssemblerState* state, char* command, char* operands, int lineNumber) {
    Opcode* cmd = findOpcode(command);
    if (!cmd) {
        fprintf(stderr, "Error in line: %d, invalid opcode: %s\n", lineNumber, command);
        return;
    }

    int srcValue, dstValue;
    int srcType = resolveOperand(state, strtok(operands, ","), &srcValue);
    int dstType = resolveOperand(state, strtok(NULL, ""), &dstValue);

    if (srcType == -1 || dstType == -1) {
        // Add to unresolved list if any operand could not be resolved
        addUnresolved(state, state->instructionCount, lineNumber, command);
    }

    // Proceed to insert words regardless of resolution status
    insertWords(state, cmd, srcType, srcValue, dstType, dstValue);
}

void finalCheck(AssemblerState* state) {
    if (state->unresolvedCount > 0) {
        fprintf(stderr, "Error: Unresolved symbols found at the end of the first pass.\n");
        for (int i = 0; i < state->unresolvedCount; i++) {
            UnresolvedInstruction* unresolved = &state->unresolved[i];
            fprintf(stderr, "Unresolved instruction in line %d: %s\n", unresolved->lineNumber, unresolved->line);
        }
        exit(EXIT_FAILURE); // Exit if unresolved symbols remain, can be changed to handle differently
    }
}

void addSymbolAndUpdateUnresolved(AssemblerState* state, Symbol newSymbol) {
    // Add the new symbol to the table
    ensureCapacity((void**)&state->symbols, &state->symbolsCapacity, state->symbolsCount + 1, sizeof(Symbol));
    state->symbols[state->symbolsCount++] = newSymbol;

    // Check unresolved instructions for any that can now be resolved
    for (int i = 0; i < state->unresolvedCount; ) {
        if (resolveInstruction(state, &state->unresolved[i])) {
            // If the instruction is resolved, remove it from the unresolved list
            state->unresolved[i] = state->unresolved[--state->unresolvedCount];  // Copy last unresolved to current position
        } else {
            i++;  // Only increment if unresolved not resolved, to skip over compacted entry
        }
    }
}

int resolveInstruction(AssemblerState* state, UnresolvedInstruction* unresolved) {
    // Attempt to resolve the instruction now that more symbols might be available
    int srcValue, dstValue;
    char* command = strtok(unresolved->line, " ");
    char* operands = strtok(NULL, "");
    int srcType = resolveOperand(state, strtok(operands, ","), &srcValue);
    int dstType = resolveOperand(state, strtok(NULL, ""), &dstValue);

    if (srcType != -1 && dstType != -1) {
        // If both operands are now resolved, re-insert the instruction
        Opcode* cmd = findOpcode(command);
        if (cmd) {
            insertWords(state, cmd, srcType, srcValue, dstType, dstValue);
            return 1; // Indicate the instruction was resolved
        }
    }
    return 0; // Indicate the instruction remains unresolved
}