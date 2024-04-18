// FileParser.c
#include "FileParser.h"
#define INITIAL_CAPACITY 10
#define CAPACITY_INCREMENT 10

// Trims leading and trailing whitespace, replaces consecutive whitespace with a single space,
// and removes any tabs between whitespace and ',' characters in the input string.
char* trimWhitespace(char* str) {
    char* dest = str; // Pointer to the destination string (modified in-place)
    int lastCharWasSpace = 0; // Flag to track if the last character was a space or tab

    // Iterate over the input string
    for (char* src = str; *src != '\0'; src++) {
        if (*src == ' ' || *src == '\t') {
            // Replace consecutive spaces or tabs with a single space
            if (!lastCharWasSpace && (*(src + 1) != ',' && *(src - 1) != ',')) {
                *dest++ = ' ';
            }
            lastCharWasSpace = 1; // Set the flag to indicate that the last character was a space or tab
        } else {
            // Copy non-space characters to the destination string
            *dest++ = *src;
            lastCharWasSpace = 0; // Reset the flag since the current character is not a space or tab
        }
    }
    *dest = '\0'; // Null-terminate the destination string

    // Trim leading whitespace
    while (isspace((unsigned char)*str)) {
        str++;
    }

    // Trim trailing whitespace
    char* end = dest - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0'; // Null-terminate the trimmed string

    // Remove any tabs in between whitespace characters before or after a ','
    dest = str;
    for (char* src = str; *src != '\0'; src++) {
        if (*src == ' ' && (*(src + 1) == ',' || *(src - 1) == ',')) {
            continue; // Skip tabs in between whitespace and ','
        }
        *dest++ = *src;
    }
    *dest = '\0'; // Null-terminate the final string

    return str; // Return a pointer to the modified string
}

// Function to parse the input file
void ParseFile(AssemblerState* state) {
    printf("Parsing file: %s\n", state->inputFilename);
    // Create the full filename by appending ".as"
    char* fullFilename = malloc(strlen(state->inputFilename) + 4); // +3 for ".as" +1 for null terminator
    sprintf(fullFilename, "%s.as", state->inputFilename);

    // Open the file with ".as" extension
    FILE* file = fopen(fullFilename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s. Try again or check the file is free for reading.\n",
                fullFilename);
        free(fullFilename);
        state->assemblerError = true;
        return; // Return with error
    }

    // Allocate memory for the dynamic list
    char** lines = malloc(INITIAL_CAPACITY * sizeof(char*));
    if (lines == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        fclose(file);
        state->assemblerError = true;
        return;
    }

    int capacity = INITIAL_CAPACITY;
    int size = 0;
    char buffer[MAX_LINE_LENGTH]; // Assuming maximum line length

    // Read lines from the file
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Trim the line
        char* trimmedLine = trimWhitespace(buffer);

        // Skip empty lines and lines starting with ';'
        if (trimmedLine[0] == '\0' || trimmedLine[0] == ';') {
            continue;
        }

        // Reallocate memory if needed
        if (size >= capacity) {
            capacity += CAPACITY_INCREMENT;
            char** temp = realloc(lines, capacity * sizeof(char*));
            if (temp == NULL) {
                fprintf(stderr, "Error reallocating memory.\n");
                fclose(file);
                free(lines);
                state->assemblerError = true;
                return;
            }
            lines = temp;
        }

        // Store the trimmed line in the list
        lines[size] = strdup(trimmedLine);
        size++;
    }

    fclose(file);
    free(fullFilename);

    state->parsedFile.lines = lines;
    state->parsedFile.numberOfLines = size;
}