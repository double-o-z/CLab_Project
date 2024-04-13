#ifndef HELPER_H
#define HELPER_H
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char** lines;
    int numberOfLines;
    const char* fileName;
} ParsedFile;

void printAllLines(ParsedFile parsedFile);
void ensureCapacity(void** array, int currentSize, int newSize, size_t elementSize);

#endif /* HELPER_H */