// Assembler.c
#include "Helper.h"
#include "FileParser.h"
#include "MacroProcessor.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "CreateOutput.h"
#include "Assembler.h"
#include "stdio.h"

void Assemble(const char* inputFilename) {
    printf("Assembling input file: %s\n", inputFilename);

    // Parse the input file
    ParsedFile parsedFile = ParseFile(inputFilename);

    // Initialize the assembler state
    AssemblerState state = initAssemblerState();

    // Process macros within the file
    ProcessMacro(&parsedFile);

    // Perform the first pass of assembly
    FirstPass(&parsedFile, &state);

    // Perform the second pass of assembly
    SecondPass(&parsedFile, &state);

    // Check if an error occurred during the second pass
    if (state.assemblerError) {
        printf("Assembly errors detected. Aborting. Fix them and try again.\n");
    } else {
        // Create output files if no errors occurred
        CreateOutput(&state, inputFilename);
        printf("\n\nFinished Assembling input file: %s\n\n\n", inputFilename);
    }

    // Free the memory allocated to the state and parsedFile
    freeAssemblerState(&state);
    freeParsedFile(&parsedFile);
}

void freeAssemblerState(AssemblerState* state) {
    // Free instruction and data arrays
    if (state->instructions.array) {
        free(state->instructions.array);
    }
    if (state->data.array) {
        free(state->data.array);
    }

    // Free symbols
    if (state->symbols) {
        for (int i = 0; i < state->symbolsCount; i++) {
            if (state->symbols[i].label) {
                free(state->symbols[i].label);
            }
        }
        free(state->symbols);
    }

    // Free externals
    if (state->externals) {
        for (int i = 0; i < state->externalsCount; i++) {
            if (state->externals[i].label) {
                free(state->externals[i].label);
            }
        }
        free(state->externals);
    }
}

void freeParsedFile(ParsedFile* parsedFile) {
    if (parsedFile->lines) {
        for (int i = 0; i < parsedFile->numberOfLines; i++) {
            if (parsedFile->lines[i]) {
                free(parsedFile->lines[i]);
            }
        }
        free(parsedFile->lines);
    }
}
