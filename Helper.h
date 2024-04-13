#ifndef HELPER_H
#define HELPER_H
#include <stdio.h>

typedef struct {
    char** lines;
    int numberOfLines;
    const char* fileName;
} ParsedFile;

void printAllLines(ParsedFile parsedFile);
char* strDuplicate(const char* str);
void ensureCapacity(void** array, int currentSize, int newSize, size_t elementSize);

#endif /* HELPER_H */