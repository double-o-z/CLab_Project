// Main.c
#include "Assembler.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s [-d] <input_file1> [<input_file2> ...]\n", argv[0]);
        return 1;
    }

    bool debugMode = false;  // Default to no debug mode
    int startIdx = 1;        // Start index for filename arguments

    // Check if the first argument is the debug flag
    if (strcmp(argv[1], "-d") == 0) {
        debugMode = true;
        startIdx = 2;  // Start processing filenames from the second index
        if (argc < 3) {  // Make sure there is at least one input file
            printf("Usage: %s [-d] <input_file1> [<input_file2> ...]\n", argv[0]);
            return 1;
        }
    }

    // Process each file
    for (int i = startIdx; i < argc; i++) {
        const char* inputFilename = argv[i];
        AssemblerState state = initAssemblerState(inputFilename, debugMode);
        Assemble(&state);  // Now pass state to the Assemble function
    }

    return 0;
}

