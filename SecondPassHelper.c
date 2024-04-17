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
            ".define", ".data", ".string", ".extern", ".entry", NULL
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
            encodeImmediateOperand(state, operand, lineNumber);
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
    printf("Encoding register operand at line %d, placement: %s\n",
           lineNumber, placement == SOURCE_OPERAND ? "source" : "destination");
    state->instructions.array[state->instructionCounter] = calculateRegisterWord(operand, placement);
    state->instructionCounter++;
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
    printf("Encoding register pair at line %d\n", lineNumber);
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

    return srcWord + destWord;  // Combine the two words
}

void encodeImmediateOperand(AssemblerState* state, char* operand, int lineNumber) {
    printf("Encoding immediate operand at line %d\n", lineNumber);
    int value = 0;
    bool valid = true; // Assume valid unless proven otherwise

    // Operand should be in the format '#X' where X is an integer or MDEFINE symbol
    if (operand[0] == '#') {
        char* valueStr = operand + 1;
        if (isdigit(valueStr[0]) || valueStr[0] == '+' || valueStr[0] == '-') {
            value = atoi(valueStr); // Convert the string to integer
        } else { // Treat as a symbol
            Symbol* symbol = findSymbolInST(state, valueStr);
            if (symbol && symbol->type == MDEFINE) {
                value = symbol->value;
            } else {
                printf("Error: Symbol '%s' not found or invalid type at line %d\n", valueStr, lineNumber);
                valid = false;
            }
        }
    }

    if (!valid) {
        state->duplicateSymbols = true; // Flag error
        value = 0; // Use 0 as a placeholder value
    }

    state->instructions.array[state->instructionCounter] = calculateImmediateWord(value);
    state->instructionCounter++;
}

void encodeDirectOperand(AssemblerState* state, char* operand, int lineNumber) {
    printf("Encoding direct operand at line %d\n", lineNumber);
    Symbol* symbol = findSymbolInST(state, operand);
    if (!symbol || symbol->type == CODE) { // Ensure symbol exists and is not of type CODE
        printf("Error: Symbol '%s' not found or invalid type at line %d\n", operand, lineNumber);
        state->duplicateSymbols = true; // Flag error
        state->instructions.array[state->instructionCounter] = 0; // Placeholder
    } else {
        state->instructions.array[state->instructionCounter] = calculateDirectWord(symbol->value, symbol->type);
    }
    state->instructionCounter++;
}

void encodeDirectIndexOperand(AssemblerState* state, char* operand, int lineNumber) {
    printf("Encoding direct index operand at line %d\n", lineNumber);
    // Operand expected to be in the format "symbol[index]"
    char* symbolPart = strtok(operand, "[");
    char* indexPart = strtok(NULL, "]");

    if (!symbolPart || !indexPart) {
        printf("Error: Invalid direct index operand format at line %d\n", lineNumber);
        state->duplicateSymbols = true;
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        return;
    }

    encodeDirectOperand(state, symbolPart, lineNumber); // Handle the symbol part
    encodeImmediateOperand(state, indexPart, lineNumber); // Handle the index part
}

// Example function to find a symbol in the symbol table
Symbol* findSymbolInST(AssemblerState* state, const char* name) {
    for (int i = 0; i < state->symbolsCount; i++) {
        if (strcmp(state->symbols[i].label, name) == 0) {
            return &state->symbols[i];
        }
    }
    return NULL;
}

int calculateDirectWord(int value, SymbolType type) {
    int result = value << 2; // Shift value left to leave room for A, R, E bits
    switch (type) {
        case MDEFINE:
        case DATA:
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
    return result;
}

int calculateImmediateWord(int value) {
    return value << 2; // Shift the immediate value left by 2 bits to align it properly
}