#include "CreateOutput.h"
#include <stdio.h>

void createOutput(AssemblerState* state) {
    /*  Implement object files creation logic here */
    if (state->assemblerError){
        printf("\n\nErrors were found for input file: %s. Fix them and run again.\n", state->inputFilename);
        return;
    }


    printf("Creating output files for: %s\n", state->inputFilename);
    createExtFile(state);
    createEntFile(state);
    createObjectFile(state);
}

void createExtFile(AssemblerState* state){
    FILE* file;
    char* extFilename;
    int i;

    if (state->externalsCount == 0){
        return;
    }

    printf("Creating externals output file.\n");
    extFilename = malloc(strlen(state->inputFilename) + 5); /*  +4 for ".ext" +1 for null terminator */
    sprintf(extFilename, "%s.ext", state->inputFilename);
    file = fopen(extFilename, "w");
    if (file != NULL) {
        for (i = 0; i < state->externalsCount; i++) {
            if (state->debugMode){
                printf("Adding line to externals file:\n"
                       "%s\t%04d\n",
                       state->externals[i].label, state->externals[i].lineNumber);
            }

            fprintf(file, "%s\t%04d\n",
                    state->externals[i].label, state->externals[i].lineNumber);
        }
        fclose(file);
        printf("Externals output file created: %s\n", extFilename);
    } else {
        printf("Failed to create externals output file: %s\n", extFilename);
    }
    free(extFilename);
}

void createEntFile(AssemblerState* state){
    FILE* file;
    char* entFilename;
    int i;

    if (!state->entriesExist){
        return;
    }

    printf("Creating entries output file.\n");
    entFilename = malloc(strlen(state->inputFilename) + 5); /*  +4 for ".ent" +1 for null terminator */
    sprintf(entFilename, "%s.ent", state->inputFilename);

    file = fopen(entFilename, "w");
    if (file != NULL) {
        for (i = 0; i < state->symbolsCount; i++) {
            if (state->symbols[i].type != ENTRY)
                continue;

            if (state->debugMode){
                printf("Adding line to entries file:\n"
                       "%s\t%04d\n",
                       state->symbols[i].label, state->symbols[i].value);
            }

            fprintf(file, "%s\t%04d\n",
                    state->symbols[i].label, state->symbols[i].value);
        }
        fclose(file);
        printf("Entries output file created: %s\n", entFilename);
    } else {
        printf("Failed to create Entries output file: %s\n", entFilename);
    }

    free(entFilename);
}

void createObjectFile(AssemblerState* state){
    FILE* file;
    int i;
    char* objectFilename, binary[15], encoded[8];
    printf("Creating object output file.\n");
    objectFilename = malloc(strlen(state->inputFilename) + 4); /*  +3 for ".ob" +1 for null terminator */
    sprintf(objectFilename, "%s.ob", state->inputFilename);
    file = fopen(objectFilename, "w");
    if (file != NULL) {
        if (state->debugMode){
            printf("Adding header line:\n"
                   "%d %d\n", state->instructions.count, state->data.count);
        }

        fprintf(file, "%d %d\n", state->instructions.count, state->data.count);

        /*  Assuming file is already opened and valid */
        for (i = 0; i < state->instructions.count; i++) {
            intToBinaryString(state->instructions.array[i], binary);
            encodeBase4(binary, encoded);
            if (state->debugMode){
                printf("Adding line to object file:\n"
                       "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION, encoded);
            }

            fprintf(file, "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION, encoded);
        }

        for (i = 0; i < state->data.count; i++) {
            intToBinaryString(state->data.array[i], binary);
            encodeBase4(binary, encoded);
            if (state->debugMode){
                printf("Adding line to object file:\n"
                       "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION + state->instructions.count, encoded);
            }

            fprintf(file, "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION + state->instructions.count, encoded);
        }

        fclose(file);
        printf("Object output file created: %s\n", objectFilename);
    } else {
        printf("Failed to create Object output file: %s\n", objectFilename);
    }

    free(objectFilename);
}

void encodeBase4(const char* binary, char* output) {
    char mapping[4] = {'*', '#', '%', '!'};
    int i, index, digit; /* i for loop, index for bit locations, digit for value of base 4 of each 2 bits */
    for (i = 0; i < 7; i++) {
        index = 2 * i;
        digit = (binary[index] - '0') * 2 + (binary[index + 1] - '0');
        output[i] = mapping[digit];
    }
    output[7] = '\0'; /*  Null-terminate the string */
}