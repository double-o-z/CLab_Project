#include "FirstPassHelper.h"

void dynamicInsertSymbol(AssemblerState* state, Symbol newSymbol) {
    // Check for existing symbol
    for (int i = 0; i < state->symbolsCount; i++) {
        if (strcmp(state->symbols[i].label, newSymbol.label) == 0) {
            fprintf(stderr, "Error: Duplicate symbol '%s' found.\n", newSymbol.label);
            state->assemblerError = true;
            return;  // Do not add the duplicate symbol, return immediately
        }
    }

    // Check if initialization is needed
    if (state->symbolsCount == 0 || state->symbols == NULL) {
        state->symbols = malloc(sizeof(Symbol));  // Allocate space for the first symbol
        if (state->symbols == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // Resize the symbols array dynamically
        Symbol* temp = realloc(state->symbols, (state->symbolsCount + 1) * sizeof(Symbol));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed during resizing!\n");
            exit(EXIT_FAILURE);
        }
        state->symbols = temp;
    }

    // Insert the new symbol
    state->symbols[state->symbolsCount++] = newSymbol;
}

void dynamicInsertExternal(AssemblerState* state, External newExternal) {
    // Check if initialization is needed
    if (state->externalsCount == 0 || state->externals == NULL) {
        state->externals = malloc(sizeof(External));  // Allocate space for the first external
        if (state->externals == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // Resize the externals array dynamically
        External * temp = realloc(state->externals, (state->externalsCount + 1) * sizeof(External));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed during resizing!\n");
            exit(EXIT_FAILURE);
        }
        state->externals = temp;
    }

    // Insert the new external
    state->externals[state->externalsCount++] = newExternal;
}

char** splitFirstWhitespace(char* str) {
    char** parts = malloc(2 * sizeof(char*));
    char* tempStr = strdup(str); // Duplicate the string
    char* firstSpace = strchr(tempStr, ' ');

    if (firstSpace != NULL) {
        *firstSpace = '\0';  // Split the duplicated string
        parts[0] = strdup(tempStr);  // Duplicate the first part
        parts[1] = strdup(firstSpace + 1);  // Duplicate the second part
    } else {
        parts[0] = strdup(tempStr);  // Duplicate the whole string as only part
        parts[1] = NULL;
    }

    free(tempStr); // Free the temporary string
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

void intToBinaryString(int value, char *buffer) {
    buffer[14] = '\0'; // Null terminator for the string
    int pos = 13;
    // Extract each bit and convert to string
    for (int i = 0; i < 14; i++) {
        if (value & (1 << i)) {
            buffer[pos--] = '1';
        } else {
            buffer[pos--] = '0';
        }
    }
}

// Function to print the data list
void printDataList(const AssemblerState* state) {
    printf("\nData List:\n");
    // Using state->data.count to access the count from DynamicArray
    for (int i = 0; i < state->data.count; i++) {
        char binaryString[15]; // 14 bits + null terminator
        intToBinaryString(state->data.array[i], binaryString);
        // Accessing data through state->data.array
        printf("Index \t%d: \tInt: \t%d%s\t Binary: \t%s\n",
               i + INDEX_FIRST_INSTRUCTION + state->instructions.count,
               state->data.array[i],
               (state->data.array[i] >= 10000000 || state->data.array[i] <= -1000000) ? "" : "\t",
               binaryString);
    }
}

// Function to print the instructions list
void printInstructionsList(const AssemblerState* state) {
    printf("\nInstructions List:\n");
    // Using state->instructions.count to access the count from DynamicArray
    for (int i = 0; i < state->instructions.count; i++) {
        char binaryString[15]; // 14 bits + null terminator
        intToBinaryString(state->instructions.array[i], binaryString);
        // Accessing instructions through state->instructions.array
        printf("Index \t%d: \tInt: \t%d%s\t Binary: \t%s\n",
               i + INDEX_FIRST_INSTRUCTION,
               state->instructions.array[i],
               (state->instructions.array[i] >= 10000000 || state->instructions.array[i] <= -1000000) ? "" : "\t",
               binaryString);
    }
}

// Helper function to find a symbol in the symbol table and return its value
int findSymbolValue(const AssemblerState* state, const char* label) {
    for (int i = 0; i < state->symbolsCount; i++) {
        if (strcmp(state->symbols[i].label, label) == 0) {
            if (state->symbols[i].type == MDEFINE) {
                return state->symbols[i].value;
            }
            else {
                fprintf(stderr, "Error: Usage of symbol '%s' is not allowed in this directive.\n", label);
            }

        }
    }
    return -1;  // Return -1 if not found, you can handle this case based on your error handling strategy
}

int to14BitTwosComplement(int value) {
    // Ensure the value is within the -8192 to 8191 range
    value = value % 16384; // Modulo 2^14 to wrap around

    // If the value is negative, adjust it to be a valid 14-bit two's complement representation
    if (value < 0) {
        value += 16384; // Adjust by 2^14
    }

    return value & 0x3FFF; // Apply a mask to ensure only the lowest 14 bits are kept
}

int createFirstWord(int srcType, int destType, int opcode) {
    int firstWord = 0;

    // Ensure srcType and destType are non-negative before masking
    srcType = (srcType >= 0) ? (srcType & 0x3) : 0;
    destType = (destType >= 0) ? (destType & 0x3) : 0;

    firstWord |= srcType << 4;  // Source type in bits 4-5
    firstWord |= destType << 2; // Destination type in bits 2-3
    firstWord |= (opcode & 0xF) << 6;  // Opcode in bits 6-9

    printf("opcode: %d\n", opcode);
    printf("opcode in bits 6-9: %d\n", (opcode & 0xF) << 6);

    return firstWord;
}

void addInstructionToInstructionList(DynamicArray* array, int instructionWord) {
    // Check if the instructions array needs to be initialized
    if (array->count == 0 || array->array == NULL) {
        array->array = malloc(sizeof(int));  // Allocate space for the first instruction
        if (array->array == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // Resize the instructions array dynamically
        int* temp = realloc(array->array, (array->count + 1) * sizeof(int));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed during resizing!\n");
            exit(EXIT_FAILURE);
        }
        array->array = temp;
    }

    // Add the new instruction to the array
    array->array[array->count++] = instructionWord;
}

void prepareOperandDataWords(AssemblerState* state, int srcType, int destType) {
    int additionalWords = 0;
    // Determine the number of words needed for source operand
    if (srcType == 2) { // Direct index
        additionalWords += 2; // Two words: one for the symbol address and one for the index value
    } else if (srcType == 0 || srcType == 1 || srcType == 3) { // Immediate, Direct, Register
        additionalWords += 1;
    }
    // Determine the number of words needed for destination operand
    if (destType == 2) { // Direct index
        additionalWords += 2; // Two words: one for the symbol address and one for the index value
    } else if (destType == 0 || destType == 1 || destType == 3) { // Immediate, Direct, Register
        additionalWords += 1;
    }
    // Special case: if both operands are registers, consolidate into a single word
    if (srcType == 3 && destType == 3) {
        additionalWords = 1; // Only one word needed because register indices are packed into a single word
    }

    printOperandTypes(srcType, destType);
    printf("additionalWords: %d\n", additionalWords);
    // Add the calculated number of additional words to the instruction list as placeholders (0)
    for (int i = 0; i < additionalWords; i++) {
        printf("adding operands instruction word\n");
        addInstructionToInstructionList(&state->instructions, 0); // Placeholder value 0
    }
}

const char* operandTypeToString(int type) {
    switch (type) {
        case 0:
            return "Immediate";
        case 1:
            return "Direct";
        case 2:
            return "Direct Index";
        case 3:
            return "Register";
        default:
            return "Unknown";  // For cases where the type is -1 or any undefined value
    }
}

void printOperandTypes(int srcType, int destType) {
    const char* srcTypeAsString = operandTypeToString(srcType);
    const char* destTypeAsString = operandTypeToString(destType);
    printf("found srcType: %s, destType: %s\n", srcTypeAsString, destTypeAsString);
}