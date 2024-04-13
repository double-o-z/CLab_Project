#include "FirstPassHelper.h"

// Initialize assembler state
AssemblerState initAssemblerState() {
    AssemblerState state;
    state.instructions.array = NULL;  // No initial allocation
    state.instructions.count = 0;     // IC initialized to 0
    state.data.array = NULL;           // No initial allocation
    state.data.count = 0;              // DC initialized to 0
    state.symbols = NULL;              // No initial allocation
    state.symbolsCount = 0;            // Symbols count initialized to 0
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
    array->array[array->count++] = value;
}

void dynamicInsertSymbol(AssemblerState* state, Symbol newSymbol) {
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
    for (int i = 0; i < state->data.count; i++) {  // Using state->data.count to access the count from DynamicArray
        char binaryString[15]; // 14 bits + null terminator
        intToBinaryString(state->data.array[i], binaryString);
        // Accessing data through state->data.array
        printf("Index %d: Int: %d, Binary: %s\n", i, state->data.array[i], binaryString);
    }
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

int to14BitTwosComplement(int value) {
    // Ensure the value is within the -8192 to 8191 range
    value = value % 16384; // Modulo 2^14 to wrap around

    // If the value is negative, adjust it to be a valid 14-bit two's complement representation
    if (value < 0) {
        value += 16384; // Adjust by 2^14
    }

    return value & 0x3FFF; // Apply a mask to ensure only the lowest 14 bits are kept
}