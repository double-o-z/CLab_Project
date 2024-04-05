// Assembler.c
#include "Assembler.h"
#include <stdio.h>

void firstPass(const char* inputFilename) {
    // Implement first pass logic here
    printf("First pass for: %s\n", inputFilename);
}

void secondPass(const char* inputFilename) {
    // Implement second pass logic here
    printf("Second pass for: %s\n", inputFilename);
}

void createObjectFile(const char* inputFilename) {
    // Implement object file creation logic here
    printf("Creating object file for: %s\n", inputFilename);
}

void assemble(const char* inputFilename) {
    firstPass(inputFilename);
    secondPass(inputFilename);
    createObjectFile(inputFilename);
}
