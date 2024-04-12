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

#endif /* HELPER_H */