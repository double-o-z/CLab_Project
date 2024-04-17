#include "Helper.h"
#include "ctype.h"

const int INDEX_FIRST_INSTRUCTION = 100;

void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", parsedFile.lines[i]);
    }
}

// Initialize assembler state
AssemblerState initAssemblerState() {
    AssemblerState state;
    state.instructions.array = NULL;  // No initial allocation
    state.instructions.count = 0;  // IC initialized to 0
    state.data.array = NULL;           // No initial allocation
    state.data.count = 0;              // DC initialized to 0
    state.symbols = NULL;              // No initial allocation
    state.symbolsCount = 0;            // Symbols count initialized to 0
    state.duplicateSymbols = false;    // Initialize the duplicate flag as false
    state.instructionCounter = 0;  // Initialize the second pass instruction counter
    return state;
}

void dynamicInsert(DynamicArray* array, int value) {
    // Check if initialization is needed
    if (array->count == 0 || array->array == NULL) {
        array->array = malloc(sizeof(int));  // Allocate space for the first element
        if (array->array == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
    } else {
        // Resize the array dynamically
        int* temp = realloc(array->array, (array->count + 1) * sizeof(int));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed during resizing!\n");
            exit(EXIT_FAILURE);
        }
        array->array = temp;
    }

    // Insert the new value
    printf("Adding to array value: %d\n", value);
    array->array[array->count++] = value;
}

// Function to print the symbols table
void printSymbolsTable(const AssemblerState* state) {
    printf("\nSymbols Table:\n\n");
    printf("Label\t\tType\t\tValue\n");
    for (int i = 0; i < state->symbolsCount; i++) {
        char* type;
        int offsetValue = 0;
        switch (state->symbols[i].type) {
            case MDEFINE: type = "mdefine"; break;
            case CODE:
                type = "code";
                offsetValue = INDEX_FIRST_INSTRUCTION;
                break;
            case DATA:
                offsetValue = INDEX_FIRST_INSTRUCTION + state->instructions.count;
                type = "data";
                break;
            case EXTERNAL: type = "extern"; break;
            case ENTRY: type = "entry"; break;
            default: type = "unknown"; break;
        }
        printf("%s\t\t%s\t\t%d\n", state->symbols[i].label, type, state->symbols[i].value + offsetValue);
    }
    printf("\nSymbols Table: %s\n",
           state->duplicateSymbols ? "Duplicate found" : "No duplicate found");
}

bool isValidInteger(const char* str) {
    // Skip the sign
    if (*str == '+' || *str == '-') str++;

    // Check for at least one digit
    if (!isdigit(*str)) return false;

    // Check all remaining characters are digits
    while (*str) {
        if (!isdigit(*str++)) return false;
    }
    return true;
}