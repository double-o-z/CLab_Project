#include "Helper.h"

void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", parsedFile.lines[i]);
    }
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