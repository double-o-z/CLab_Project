#include "SecondPassHelper.h"

bool isDirective(char* line) {
    // Trim leading whitespace
    while (isspace((unsigned char)*line)) line++;

    // Extract the first word from the line to check against known directives
    char firstWord[100]; // Adjust size based on possible maximum length of directive
    size_t index = 0;
    while (!isspace((unsigned char)*line) && *line != '\0' && index < (sizeof(firstWord) - 1)) {
        firstWord[index++] = *line++;
    }
    firstWord[index] = '\0';

    // List of known directive commands
    const char* directives[] = {
            ".define", ".data", ".string", ".extern", NULL
    };

    for (const char** dir = directives; *dir != NULL; dir++) {
        if (strcmp(firstWord, *dir) == 0) {
            return true;
        }
    }

    return false;
}

void encodeOperandDataWords(AssemblerState* state, int srcType, int destType, char* operands, int lineNumber) {
    if (operands == NULL || strlen(operands) == 0) {
        // No operands to process
        printf("No operands at line %d\n", lineNumber);
        return;
    }

    // Split operands into src and dest
    char* srcOperand = NULL;
    char* destOperand = NULL;
    char* comma = strchr(operands, ',');
    if (comma != NULL) {
        *comma = '\0';  // Terminate the srcOperand string
        srcOperand = operands;
        destOperand = comma + 1;
    } else {
        destOperand = operands;  // Only one operand, it's a destination operand
    }

    // Special handling if both operands are registers
    if (srcType == 3 && destType == 3) {
        encodeRegisterPair(state, srcOperand, destOperand, lineNumber);
    } else {
        // Encode source operand if it exists and is not combined with a destination register
        if (srcType != -1) {
            encodeOperandByType(state, srcType, srcOperand, lineNumber, SOURCE_OPERAND);
        }
        // Encode destination operand if it exists
        if (destType != -1) {
            encodeOperandByType(state, destType, destOperand, lineNumber, DESTINATION_OPERAND);
        }
    }

    printOperandTypes(srcType, destType);
}

void encodeOperandByType(AssemblerState* state, int type, char* operand, int lineNumber, OperandPlacement placement) {
    switch (type) {
        case 0: // Immediate
            encodeImmediateOperand(state, operand, lineNumber, false);
            break;
        case 1: // Direct
            encodeDirectOperand(state, operand, lineNumber);
            break;
        case 2: // Direct Index
            encodeDirectIndexOperand(state, operand, lineNumber);
            break;
        case 3: // Register
            encodeRegisterOperand(state, operand, lineNumber, placement);
            break;
        default:
            printf("Error on line %d: Invalid operand type %d\n", lineNumber, type);
            break;
    }
}

void encodeRegisterOperand(AssemblerState* state, char* operand, int lineNumber, OperandPlacement placement) {
    int value = calculateRegisterWord(operand, placement);
    state->instructions.array[state->instructionCounter] = value;
    state->instructionCounter++;
    printf("Encoding register operand at line %d, placement: %s, and value: %d\n",
           lineNumber + INDEX_FIRST_INSTRUCTION, placement == SOURCE_OPERAND ? "source" : "destination", value);
}

int calculateRegisterWord(const char* operand, OperandPlacement placement) {
    int regIndex = operand[1] - '0';
    if (placement == SOURCE_OPERAND) {
        return regIndex << 5;  // Shift for source register
    } else {
        return regIndex << 2;  // Shift for destination register
    }
}

void encodeRegisterPair(AssemblerState* state, char* srcOperand, char* destOperand, int lineNumber) {
    printf("Encoding register pair at line %d\n", lineNumber + INDEX_FIRST_INSTRUCTION);
    // Logic to encode a pair of register operands into a single word
    int regPairValue = calculateRegisterPairWord(srcOperand, destOperand); // Assume function to calculate the encoded word
    state->instructions.array[state->instructionCounter] = regPairValue;
    state->instructionCounter++;
}

int calculateRegisterPairWord(const char* srcOperand, const char* destOperand) {
    // Assumes srcOperand and destOperand are of the form "rX" where X is the register index
    int srcIndex = srcOperand[1] - '0';  // Convert char to int
    int destIndex = destOperand[1] - '0';  // Convert char to int

    int srcWord = srcIndex << 5;  // Shift source index left by 5 bits
    int destWord = destIndex << 2;  // Shift destination index left by 2 bits
    int result = srcWord + destWord;  // Combine the two words
    printf("Result of calculateRegisterPairWord: %d\n", result);
    return result;
}

void encodeImmediateOperand(AssemblerState* state, char* operand, int lineNumber, bool isSubOperand) {
    printf("Encoding immediate operand: %s, at line %d\n", operand, lineNumber + INDEX_FIRST_INSTRUCTION);
    int value = 0;
    bool valid = true; // Assume valid unless proven otherwise

    // Operand should be in the format '#X' where X is an integer or MDEFINE symbol
    if (operand[0] == '#' || isSubOperand) {
        // If called on main operand, start parsing after # sign.
        // If called as index of list operand, there will be no #, so start immediately to parse.
        char* valueStr = operand + 1 - isSubOperand;
        if (isdigit(valueStr[0]) || valueStr[0] == '+' || valueStr[0] == '-') {
            value = atoi(valueStr); // Convert the string to integer
        } else { // Treat as a symbol
            Symbol* symbol = findSymbolInST(state, valueStr);
            if (symbol && symbol->type == MDEFINE) {
                value = symbol->value;
            } else {
                printf("Error: Symbol '%s' not found or invalid type at line %d\n",
                       valueStr, lineNumber + INDEX_FIRST_INSTRUCTION);
                valid = false;
            }
        }
    }

    if (!valid) {
        state->assemblerError = true; // Flag error decoding operand
        value = 0;                    // Use 0 as a placeholder value
    }

    state->instructions.array[state->instructionCounter] = calculateImmediateWord(value);
    state->instructionCounter++;
}

void encodeDirectOperand(AssemblerState* state, char* operand, int lineNumber) {
    printf("Encoding direct operand: %s, at line: %d\n", operand, lineNumber + INDEX_FIRST_INSTRUCTION);
    Symbol* symbol = findSymbolInST(state, operand);
    if (symbol != NULL){
        printf("With symbol type: %s, and value: %d\n", symbolTypeToString(symbol->type), symbol->value);
        if (symbol->type == EXTERNAL) {
            addExternalLocation(state, symbol->label, lineNumber);
        }
    }

    // Ensure symbol exists and is of type DATA\ENTRY\EXTERNAL
    if (!symbol || symbol->type == MDEFINE) {
        printf("Error: Symbol '%s' not found or invalid type at line %d\n",
               operand, lineNumber + INDEX_FIRST_INSTRUCTION);
        state->assemblerError = true; // Error misuse of operand - using incompatible variable as list.
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
    } else {
        state->instructions.array[state->instructionCounter++] = calculateDirectWord(symbol->value,
                                                                                     symbol->type);
    }
}

void addExternalLocation(AssemblerState* state, char* label, int lineNumber) {
    // Logic to add line and symbol name to externalLocations list
    External newExternal = {strdup(label), lineNumber};
    dynamicInsertExternal(state, newExternal);
}

void encodeDirectIndexOperand(AssemblerState* state, char* operand, int lineNumber) {
    printf("Encoding direct index operand at line %d\n", lineNumber + INDEX_FIRST_INSTRUCTION);
    // Operand expected to be in the format "symbol[index]"
    char* symbolPart = strtok(operand, "[");
    char* indexPart = strtok(NULL, "]");
    printf("With symbolPart: %s, and indexPart: %s\n", symbolPart, indexPart);
    if (!symbolPart || !indexPart) {
        printf("Error: Invalid direct index operand format at line %d\n", lineNumber + INDEX_FIRST_INSTRUCTION);
        state->assemblerError = true;
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        return;
    }

    encodeDirectOperand(state, symbolPart, lineNumber); // Handle the symbol part
    encodeImmediateOperand(state, indexPart, lineNumber, true); // Handle the index part
}

int calculateDirectWord(int value, SymbolType type) {
    int result = value << 2; // Shift value left to leave room for A, R, E bits
    switch (type) {
        case DATA:
            result |= 2; // Binary 10: A = 1, R = 0, E = 0 (local)
            break;
        case CODE:
            result |= 2; // Binary 10: A = 1, R = 0, E = 0 (local)
            break;
        case ENTRY:
            result |= 2; // Binary 10: might be used differently in linking
            break;
        case EXTERNAL:
            result |= 1; // Binary 01: A = 0, R = 0, E = 1 (external)
            break;
        default:
            // Default case, typically shouldn't happen unless there's an error in logic
            printf("Unexpected symbol type for direct word calculation.\n");
            break;
    }
    printf("Result of calculateDirectWord: %d\n", result);
    return result;
}

int calculateImmediateWord(int value) {
    // Shift the immediate value left by 2 bits to align it properly
    int result = value << 2;
    printf("Result of calculateImmediateWord: %d\n", result);
    return result;
}