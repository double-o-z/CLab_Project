#include "Opcodes.h"
#include "stdarg.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"

// Initialize with allowable types for each opcode
Opcode opcodes[] = {
        {"mov", 0, {0, 1, 2, 3, -1}, {1, 2, 3, -1}},
        {"cmp", 1, {0, 1, 2, 3, -1}, {0, 1, 2, 3, -1}},
        {"add", 2, {0, 1, 2, 3, -1}, {1, 2, 3, -1}},
        {"sub", 3, {0, 1, 2, 3, -1}, {1, 2, 3, -1}},
        {"not", 4, {-1}, {1, 2, 3, -1}},
        {"clr", 5, {-1}, {1, 2, 3, -1}},
        {"lea", 6, {1, 2, -1}, {1, 2, 3, -1}},
        {"inc", 7, {-1}, {1, 2, 3, -1}},
        {"dec", 8, {-1}, {1, 2, 3, -1}},
        {"jmp", 9, {-1}, {1, 3, -1}},
        {"bne", 10, {-1}, {1, 3, -1}},
        {"red", 11, {-1}, {1, 2, 3, -1}},
        {"prn", 12, {-1}, {0, 1, 2, 3, -1}},
        {"jsr", 13, {-1}, {1, 3, -1}},
        {"rts", 14, {-1}, {-1}},
        {"hlt", 15, {-1}, {-1}}
};

int findOpcodeIndex(const char* command) {
    for (size_t i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); i++) {
        if (strcmp(opcodes[i].name, command) == 0) {
            return i;
        }
    }
    return -1;  // Return -1 if no matching opcode is found
}

// Helper function to initialize operand type arrays
void initOperandTypes(int *types, int num, ...) {
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num; ++i) {
        types[i] = va_arg(args, int);
    }
    if (num < 4) {
        types[num] = -1; // Mark the end of valid types with -1
    }
    va_end(args);
}

bool parseOperands(AssemblerState* state, int* srcType, int* destType, const char* operands, Opcode opcode) {
    char operandStr[100]; // Buffer to hold a copy of operands
    if (operands != NULL) {
        strncpy(operandStr, operands, sizeof(operandStr) - 1);
        operandStr[sizeof(operandStr) - 1] = '\0';  // Ensure null termination
    } else {
        operandStr[0] = '\0';  // Set to empty string if operands are NULL
    }

    char* token = strtok(operandStr, ",");
    *srcType = -1;
    *destType = -1;
    int pos = 0;

    // Track the number of operands processed
    int numOperands = 0;

    while (token != NULL) {
        int type = determineOperandType(state, token);
        if (pos == 0) {
            *srcType = type; // Initially assume first token as source
        } else if (pos == 1) {
            *destType = type; // Second token, if exists, is destination
        }
        token = strtok(NULL, ",");
        pos++;
        numOperands++;
    }

    // If there is only one operand, treat it as the destination operand
    if (numOperands == 1) {
        *destType = *srcType;
        *srcType = -1; // Reset source type since it's actually not provided
    }

    if (state->debugMode){
        printf("srcType: %d, destType: %d\n", *srcType, *destType);
    }

    // Validate operand types
    return isValidType(*srcType, opcode.sourceTypes) &&
    isValidType(*destType, opcode.destTypes);
}

bool isValidType(int type, const int* allowedTypes) {
    int i;
    for (i = 0; allowedTypes[i] != -1; i++) {
        if (type == allowedTypes[i]) {
            return true;
        }
    }
    if (type == -1)
        // If no type was assigned (type == -1), it's valid if no operand is expected
        return i == 0;

    return false;  // type == -2 means invalid operand type
}

bool isDirectIndexOperand(const char* operand) {
    // Check if operand starts with a letter and contains both '[' and ']'
    if (!isalpha(operand[0]) || strchr(operand, '[') == NULL || strchr(operand, ']') == NULL) {
        return false;
    }

    // Check for correct structure: Symbol[Symbol or Integer]
    char symbol[100];
    char index[100];
    if (sscanf(operand, "%[^[][%[^]]]", symbol, index) != 2) {
        return false;
    }

    // Check that there's nothing after ']'
    size_t closingBracketPos = strchr(operand, ']') - operand;
    if (operand[closingBracketPos + 1] != '\0') {
        return false;
    }

    // Check symbol is a valid Direct type symbol
    if (!isValidDirectOperand(symbol))
        return false;

    // Check the index part to be either a symbol or an integer
    // Start with a letter implies it's a symbol (assuming type MDEFINE is checked during the second pass)
    if (isalpha(index[0])) {
        return isValidDirectOperand(index);
    } else { // Otherwise, check if it's a valid integer
        char* end;
        strtol(index, &end, 10);
        return (*end == '\0'); // Ensure the entire index string is a number
    }
}

// find a symbol in the symbol table
Symbol* findSymbolInST(AssemblerState* state, const char* name) {
    for (int i = 0; i < state->symbolsCount; i++) {
        if (strcmp(state->symbols[i].label, name) == 0) {
            return &state->symbols[i];
        }
    }
    return NULL;
}

int determineOperandType(AssemblerState* state, const char* operand) {
    if (operand == NULL) {
        return -1;  // No operand - might be valid depending on opcode.
    } else if (operand[0] == '#') {
        // Immediate operand, ensure valid integer follows the '#'
        if (isValidInteger(operand + 1)) {
            return 0; // Valid immediate operand as int.
        } else { // Treat as a symbol
            Symbol* symbol = findSymbolInST(state, operand + 1);
            if (symbol && symbol->type == MDEFINE) {
                return 0;
            } else {
                return -2;
            }
        }
    } else if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' && operand[1] <= '7') {
        return 3;  // Register operand
    } else if (isDirectIndexOperand(operand)) {
        return 2;  // Direct Index operand
    } else if (isValidDirectOperand(operand)) {
        return 1;  // Direct operand
    }
    return -2;  // Invalid operand type
}

bool isValidDirectOperand(const char* str) {
    // Ensure the first character is alphabetic
    if (!isalpha(*str)) return false;

    // Check all characters are alphanumeric
    str++;  // Move past the first character
    while (*str) {
        if (!isalnum(*str++)) return false;
    }
    return true;
}