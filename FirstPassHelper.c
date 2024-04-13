#include "FirstPassHelper.h"

// Helper function to ensure capacity for dynamic arrays
void ensureCapacity(void** array, int currentSize, int newSize, size_t elementSize) {
    if (currentSize >= newSize) {
        newSize += 10;
        *array = realloc(*array, newSize * elementSize);
        if (*array == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
    }
}

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

// Process a line to determine its type and handle labels
void processLine(AssemblerState* state, char* line) {
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

    free(parts);  // Free parts array after use
}