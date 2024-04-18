#include "SecondPassHelper.h"

bool isDirective(char* command) {
    // List of known directive commands
    const char* directives[] = {
            ".define", ".data", ".string", ".extern", NULL
    };

    // Check if the command matches any known directives
    for (const char** dir = directives; *dir != NULL; dir++) {
        if (strcmp(command, *dir) == 0) {
            return true;
        }
    }

    return false;
}

void encodeOperandDataWords(AssemblerState* state, int srcType, int destType, char* operands) {
    if (operands == NULL || strlen(operands) == 0) {
        // No operands to process, but it's okay. It's supposed to be this way.
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

    // Print operand types.
    if (state->debugMode){
        printOperandTypes(srcType, destType);
    }

    // Special handling if both operands are registers
    if (srcType == 3 && destType == 3) {
        encodeRegisterPair(state, srcOperand, destOperand);
    } else {
        // Encode source operand if it exists and is not combined with a destination register
        if (srcType != -1) {
            encodeOperandByType(state, srcType, srcOperand, SOURCE_OPERAND);
        }
        // Encode destination operand if it exists
        if (destType != -1) {
            encodeOperandByType(state, destType, destOperand, DESTINATION_OPERAND);
        }
    }
}

void encodeOperandByType(AssemblerState* state, int type, char* operand, OperandPlacement placement) {
    switch (type) {
        case 0: // Immediate
            encodeImmediateOperand(state, operand, false);
            break;
        case 1: // Direct
            encodeDirectOperand(state, operand);
            break;
        case 2: // Direct Index
            encodeDirectIndexOperand(state, operand);
            break;
        case 3: // Register
            encodeRegisterOperand(state, operand, placement);
            break;
        default:
            printf("Error on line %d: Invalid operand type %d\n",
                   state->parsedFile.currentLineNum + 1, type);
            state->assemblerError = true;
            break;
    }
}

void encodeRegisterOperand(AssemblerState* state, char* operand, OperandPlacement placement) {
    int value = calculateRegisterWord(state, operand, placement);
    if (state->debugMode){
        printf("Encoding register operand at instruction %d, placement: %s, and value: %d\n",
               state->instructionCounter + INDEX_FIRST_INSTRUCTION,
               placement == SOURCE_OPERAND ? "source" : "destination", value);
    }

    state->instructions.array[state->instructionCounter] = value;
    state->instructionCounter++;
}

int calculateRegisterWord(AssemblerState* state, const char* operand, OperandPlacement placement) {
    int regIndex = operand[1] - '0';
    int result;
    if (placement == SOURCE_OPERAND) {
        result = regIndex << 5;  // Shift for source register
    } else {
        result = regIndex << 2;  // Shift for destination register
    }

    if (state->debugMode){
        printf("Result of calculateRegisterWord: %d\n", result);
    }

    return result;
}

void encodeRegisterPair(AssemblerState* state, char* srcOperand, char* destOperand) {
    if (state->debugMode){
        printf("Encoding register pair at instruction %d\n", state->instructionCounter + INDEX_FIRST_INSTRUCTION);
    }

    // Logic to encode a pair of register operands into a single word
    // Assume function to calculate the encoded word
    int regPairValue = calculateRegisterPairWord(state, srcOperand, destOperand);
    state->instructions.array[state->instructionCounter] = regPairValue;
    state->instructionCounter++;
}

int calculateRegisterPairWord(AssemblerState* state, const char* srcOperand, const char* destOperand) {
    // Assumes srcOperand and destOperand are of the form "rX" where X is the register index
    int srcIndex = srcOperand[1] - '0';  // Convert char to int
    int destIndex = destOperand[1] - '0';  // Convert char to int
    int srcWord = srcIndex << 5;  // Shift source index left by 5 bits
    int destWord = destIndex << 2;  // Shift destination index left by 2 bits
    int result = srcWord + destWord;  // Combine the two words
    if (state->debugMode){
        printf("Result of calculateRegisterPairWord: %d\n", result);
    }

    return result;
}

void encodeImmediateOperand(AssemblerState* state, char* operand, bool isSubOperand) {
    if (state->debugMode){
        printf("Encoding immediate operand: %s, for instruction number %d\n",
               operand, state->instructionCounter + INDEX_FIRST_INSTRUCTION);
    }

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
                printf("Error: Symbol '%s' not found or invalid type at instruction %d\n",
                       valueStr, state->instructionCounter + INDEX_FIRST_INSTRUCTION);
                valid = false;
                state->assemblerError = true;
            }
        }
    }

    if (!valid) {
        state->assemblerError = true; // Flag error decoding operand
        value = 0;                    // Use 0 as a placeholder value
    }

    state->instructions.array[state->instructionCounter] = calculateImmediateWord(state, value);
    state->instructionCounter++;
}

void encodeDirectOperand(AssemblerState* state, char* operand) {
    if (state->debugMode){
        printf("Encoding direct operand: %s, for instruction number %d\n",
               operand, state->instructionCounter + INDEX_FIRST_INSTRUCTION);
    }

    Symbol* symbol = findSymbolInST(state, operand);
    if (symbol != NULL){
        if (state->debugMode){
            printf("With symbol type: %s, and value: %d\n", symbolTypeToString(symbol->type), symbol->value);
        }

        if (symbol->type == EXTERNAL) {
            addExternalLocation(state, symbol->label);
        }
    }

    // Ensure symbol exists and is of type DATA\ENTRY\EXTERNAL\CODE
    if (!symbol || symbol->type == MDEFINE) {
        printf("Error: Symbol '%s' not found or invalid type at line %d\n",
               operand, state->parsedFile.currentLineNum + 1);
        state->assemblerError = true; // Error misuse of operand - using incompatible variable as list.
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
    } else {
        state->instructions.array[state->instructionCounter++] = calculateDirectWord(state,
                                                                                     symbol->value,
                                                                                     symbol->type);
    }
}

void addExternalLocation(AssemblerState* state, char* label) {
    // Logic to add instruction index and symbol name to externalLocations list
    if (state->debugMode){
        printf("Adding external: %s, for instruction number: %d\n",
               label, state->instructionCounter + INDEX_FIRST_INSTRUCTION);
    }

    External newExternal = {strdup(label), state->instructionCounter + INDEX_FIRST_INSTRUCTION};
    dynamicInsertExternal(state, newExternal);
}

void encodeDirectIndexOperand(AssemblerState* state, char* operand) {
    if (state->debugMode){
        printf("Encoding direct index operand: %s, for instruction number %d\n",
               operand, state->instructionCounter + INDEX_FIRST_INSTRUCTION);
    }

    // Operand expected to be in the format "symbol[index]"
    char* symbolPart = strtok(operand, "[");
    char* indexPart = strtok(NULL, "]");
    if (state->debugMode){
        printf("With symbolPart: %s, and indexPart: %s\n", symbolPart, indexPart);
    }

    if (!symbolPart || !indexPart) {
        printf("Error: Invalid direct index operand format at line %d\n",
               state->parsedFile.currentLineNum + 1);
        state->assemblerError = true;
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        state->instructions.array[state->instructionCounter++] = 0; // Placeholder
        return;
    }

    encodeDirectOperand(state, symbolPart); // Handle the symbol part
    encodeImmediateOperand(state, indexPart, true); // Handle the index part
}

int calculateDirectWord(AssemblerState* state, int value, SymbolType type) {
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
            printf("Unexpected symbol type for direct word calculation, at line: %d.\n",
                   state->parsedFile.currentLineNum + 1);
            state->assemblerError = true;
            break;
    }

    if (state->debugMode){
        printf("Result of calculateDirectWord: %d\n", result);
    }

    return result;
}

int calculateImmediateWord(AssemblerState* state, int value) {
    // Shift the immediate value left by 2 bits to align it properly
    int result = value << 2;
    if (state->debugMode){
        printf("Result of calculateImmediateWord: %d\n", result);
    }

    return result;
}