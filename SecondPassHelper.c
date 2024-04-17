#include "SecondPassHelper.h"

bool isDirective(char* line) {
    // Trim leading whitespace
    while (isspace((unsigned char)*line)) line++;

    // Extract the first word from the line to check against known directives
    char firstWord[100]; // Adjust size based on possible maximum length of directive
    int index = 0;
    while (!isspace((unsigned char)*line) && *line != '\0' && index < (sizeof(firstWord) - 1)) {
        firstWord[index++] = *line++;
    }
    firstWord[index] = '\0';

    // List of known directive commands
    const char* directives[] = {
            ".define", ".data", ".string", ".extern", ".entry", NULL
    };

    for (const char** dir = directives; *dir != NULL; dir++) {
        if (strcmp(firstWord, *dir) == 0) {
            return true;
        }
    }

    return false;
}