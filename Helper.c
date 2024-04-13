#include "Helper.h"


void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", parsedFile.lines[i]);
    }
}

// Helper function to ensure capacity for dynamic arrays
void ensureCapacity(void** array, int* currentCapacity, int requiredSize, size_t elementSize) {
    if (*currentCapacity < requiredSize) {
        int newCapacity = requiredSize + 10;  // Increase capacity beyond the required size to reduce the number of reallocations
        void* newArray = realloc(*array, newCapacity * elementSize);
        if (newArray == NULL) {
            fprintf(stderr, "Memory allocation failed!\n");
            exit(EXIT_FAILURE);
        }
        *array = newArray;
        *currentCapacity = newCapacity;  // Update the capacity variable
    }
}