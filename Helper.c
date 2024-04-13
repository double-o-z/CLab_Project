#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", parsedFile.lines[i]);
    }
}

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