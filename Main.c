// Main.c
#include "Assembler.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <input_file1> [<input_file2> ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        const char* inputFilename = argv[i];
        Assemble(inputFilename);
    }

    return 0;
}

