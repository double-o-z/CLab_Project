#include "Helper.h"
#include "ctype.h"

const int INDEX_FIRST_INSTRUCTION = 100;

// Initialize assembler state
AssemblerState initAssemblerState(const char* inputFilename, bool debugMode) {
    AssemblerState state;
    state.inputFilename = inputFilename;  // Receive input file name from main.
    state.instructions.array = NULL;      // No initial allocation
    state.instructions.count = 0;         // IC initialized to 0
    state.data.array = NULL;              // No initial allocation
    state.data.count = 0;                 // DC initialized to 0
    state.symbols = NULL;                 // No initial allocation
    state.symbolsCount = 0;               // Symbols count initialized to 0
    state.assemblerError = false;         // Initialize the duplicate flag as false
    state.instructionCounter = 0;         // Initialize the second pass instruction counter
    state.externals = NULL;               // No initial allocation
    state.externalsCount = 0;             // Externals count initialized to 0
    state.entriesExist = false;           // Default is false, until a valid entry directive is handled.
    state.debugMode = debugMode;          // Receive debugMode from main.
    state.parsedFile.lines = NULL;        // Parsed file's lines.
    state.parsedFile.numberOfLines = 0;   // Parsed file's number of lines.
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
void printSymbolsTable(AssemblerState* state) {
    printf("\nSymbols Table:\n\n");
    printf("Label\t\tType\t\tValue\n");
    for (int i = 0; i < state->symbolsCount; i++) {
        char* type;
        switch (state->symbols[i].type) {
            case MDEFINE:  type = "mdefine"; break;
            case CODE:     type = "code";    break;
            case DATA:     type = "data";    break;
            case EXTERNAL: type = "extern";  break;
            case ENTRY:    type = "entry";   break;
            default:       type = "unknown"; break;
        }
        printf("%s\t\t%s\t\t%d\n", state->symbols[i].label, type, state->symbols[i].value);
    }
}

// Function to print the symbols table
void printExternalsTable(AssemblerState* state) {
    printf("\nExternals Table:\n\n");
    printf("Label\t\tMem Line\n");
    for (int i = 0; i < state->externalsCount; i++) {
        printf("%s\t\t%d\n", state->externals[i].label, state->externals[i].lineNumber);
    }
}

void printAllLines(AssemblerState* state) {
    printf("parsedFile contains %d lines:\n\n", state->parsedFile.numberOfLines);
    for (int i = 0; i < state->parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", state->parsedFile.lines[i]);
    }
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

const char* symbolTypeToString(int type) {
    switch (type) {
        case 0:
            return "MDEFINE";
        case 1:
            return "CODE";
        case 2:
            return "DATA";
        case 3:
            return "EXTERNAL";
        default:
            return "ENTRY";  // For cases where the type is -1 or any undefined value
    }
}