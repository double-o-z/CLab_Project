#include "FirstPass.h"

// Main function to start the first pass
void FirstPass(ParsedFile* parsedFile) {
    printf("First pass for: %s\n", parsedFile->fileName);
    AssemblerState state = initAssemblerState();
    for (int i = 0; i < parsedFile->numberOfLines; i++) {
        processLine(&state, parsedFile->lines[i]);
    }

    // Further processing, error checks, etc. (to be implemented)
}