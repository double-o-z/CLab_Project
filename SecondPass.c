#include "SecondPass.h"
#include <stdio.h>

void SecondPass(ParsedFile* parsedFile, AssemblerState* state) {
    // Implement second pass logic here
    printf("Second pass for: %s\n", parsedFile->fileName);

    if (state->duplicateSymbols){
        printf("Duplicate symbols found in this file, aborting second pass.\n");
        return;
    }

    printf("Second pass starting logic\n");
}