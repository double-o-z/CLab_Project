// FileParser.c
#include "FileParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 10
#define CAPACITY_INCREMENT 10

// Function to duplicate a string, and dynamically assign memory to the new string, by its length.
char* strDuplicate(const char* str) {
    size_t len = strlen(str) + 1;
    char* new_str = malloc(len);
    if (new_str == NULL) {
        return NULL;
    }
    memcpy(new_str, str, len);
    return new_str;
}

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
ParsedFile ParseFile(const char* filename) {
    ParsedFile result = { .lines = NULL, .numberOfLines = 0 };
    printf("Parsing file: %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return result;
    }

    // Allocate memory for the dynamic list
    char** lines = malloc(INITIAL_CAPACITY * sizeof(char*));
    if (lines == NULL) {
        fprintf(stderr, "Error allocating memory.\n");
        fclose(file);
        return result;
    }
    int capacity = INITIAL_CAPACITY;
    int size = 0;

    char buffer[256]; // Assuming maximum line length of 256 characters

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
                result.lines = NULL; // Set to NULL to indicate an error
                result.numberOfLines = 0;
                return result;
            }
            lines = temp;
        }

        // Store the trimmed line in the list
        lines[size] = strDuplicate(trimmedLine);
        size++;
    }

    fclose(file);

    // Iterate over the parsed lines and print them for clarity
    for (int i = 0; i < size; i++) {
        printf("Line %d: %s\n", i + 1, lines[i]);
    }

    result.lines = lines;
    result.numberOfLines = size;
    return result;
}