/*  Assembler.c */
#include "Helper.h"
#include "FileParser.h"
#include "MacroProcessor.h"
#include "FirstPass.h"
#include "SecondPass.h"
#include "CreateOutput.h"
#include "Assembler.h"
#include "stdio.h"

void assembleProcess(AssemblerState* state) {
    printf("\nAssembling input file: %s\n", state->inputFilename);

    /*  Parse the input file */
    parseFile(state);

    /*  Check if an error occurred during the file parsing, if so, no point in continuing. */
    if (state->assemblerError) {
        printf("Assembly errors detected. Aborting. Fix them and try again.\n\n");
    } else {
        /*  Process macros within the file */
        processMacro(state);

        /*  Perform the first pass of assembly */
        firstPass(state);

        /*  Perform the second pass of assembly */
        secondPass(state);

        /*  Check if an error occurred during the second pass */
        if (state->assemblerError) {
            printf("Assembly errors detected. Aborting. Fix them and try again.\n\n");
        } else {
            /*  Create output files if no errors occurred */
            createOutput(state);
            printf("Finished Assembling input file: %s\n\n", state->inputFilename);
        }
    }

    /*  Free the memory allocated to the state and parsedFile */
    freeAssemblerState(state);
}

void freeAssemblerState(AssemblerState* state) {
    /*  Free parsedFile data. */
    if (state->parsedFile.lines) {
        int i;
        for (i = 0; i < state->parsedFile.numberOfLines; i++) {
            if (state->parsedFile.lines[i]) {
                free(state->parsedFile.lines[i]);
            }
        }
        free(state->parsedFile.lines);
    }

    /*  Free instruction and data arrays */
    if (state->instructions.array) {
        free(state->instructions.array);
    }
    if (state->data.array) {
        free(state->data.array);
    }

    /*  Free symbols */
    if (state->symbols) {
        int i;
        for (i = 0; i < state->symbolsCount; i++) {
            if (state->symbols[i].label) {
                free(state->symbols[i].label);
            }
        }
        free(state->symbols);
    }

    /*  Free externals */
    if (state->externals) {
        int i;
        for (i = 0; i < state->externalsCount; i++) {
            if (state->externals[i].label) {
                free(state->externals[i].label);
            }
        }
        free(state->externals);
    }

    if (state->debugMode) {
        printf("Freed all allocated memory.\n");
    }
}
