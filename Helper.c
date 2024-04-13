#include "Helper.h"

void printAllLines(ParsedFile parsedFile) {
    printf("parsedFile contains %d lines:\n\n", parsedFile.numberOfLines);
    for (int i = 0; i < parsedFile.numberOfLines; i++) {
        printf("\t%s\n\n", parsedFile.lines[i]);
    }
}