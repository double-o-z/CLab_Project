#include "FirstPass.h"

// Main function to start the first pass
void FirstPass(ParsedFile* parsedFile) {
    printf("First pass for: %s\n", parsedFile->fileName);
    AssemblerState state = initAssemblerState();
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        // Pass the current line number, i, to processLine; line numbers start at 1 for user clarity
        processLine(&state, parsedFile->lines[i], i + 1);
    }

    // Call helper functions to print symbols table and data list
    printSymbolsTable(&state);
    printDataList(&state);

    // Further processing, error checks, etc. (to be implemented)
}