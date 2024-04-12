#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("Line %d: %s\n", i + 1, parsedFile.lines[i]);
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