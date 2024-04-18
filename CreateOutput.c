#include "CreateOutput.h"
#include <stdio.h>

void CreateOutput(AssemblerState* state, const char* fileName) {
    // Implement object files creation logic here
    if (state->assemblerError)
        exit(1);

    printf("\n\nCreating object files for: %s\n", fileName);
    createExtFile(state, fileName);
    createEntFile(state, fileName);
    createObjectFile(state, fileName);
}

void createExtFile(AssemblerState* state, const char* fileName){
    if (state->externalsCount == 0)
        return;

    printf("Creating externals output file.\n\n");

    char* extFilename = malloc(strlen(fileName) + 5); // +4 for ".ext" +1 for null terminator
    sprintf(extFilename, "%s.ext", fileName);

    FILE* file = fopen(extFilename, "w");
    if (file != NULL) {
        for (int i = 0; i < state->externalsCount; i++) {
            printf("Adding line to externals file:\n"
                   "%s\t%04d\n",
                   state->externals[i].label, state->externals[i].lineNumber);
            fprintf(file, "%s\t%04d\n",
                    state->externals[i].label, state->externals[i].lineNumber);
        }
        fclose(file);
        printf("\nExternals output file created: %s\n\n", extFilename);
    } else {
        printf("Failed to create externals output file: %s\n", extFilename);
    }
    free(extFilename);
}

void createEntFile(AssemblerState* state, const char* fileName){
    if (!state->entriesExist)
        return;

    printf("Creating entries output file.\n");

    char* entFilename = malloc(strlen(fileName) + 5); // +4 for ".ent" +1 for null terminator
    sprintf(entFilename, "%s.ent", fileName);

    FILE* file = fopen(entFilename, "w");
    if (file != NULL) {
        for (int i = 0; i < state->symbolsCount; i++) {
            if (state->symbols[i].type != ENTRY)
                continue;

            printf("Adding line to entries file:\n"
                   "%s\t%04d\n",
                   state->symbols[i].label, state->symbols[i].value);
            fprintf(file, "%s\t%04d\n",
                    state->symbols[i].label, state->symbols[i].value);
        }
        fclose(file);
        printf("\nEntries output file created: %s\n\n", entFilename);
    } else {
        printf("Failed to create Entries output file: %s\n", entFilename);
    }
    free(entFilename);
}

void createObjectFile(AssemblerState* state, const char* fileName){
    printf("Creating entries output file.\n");

    char* objectFilename = malloc(strlen(fileName) + 4); // +3 for ".ob" +1 for null terminator
    sprintf(objectFilename, "%s.ob", fileName);

    FILE* file = fopen(objectFilename, "w");
    if (file != NULL) {
        printf("Adding header line:\n"
               "%d %d\n", state->instructions.count, state->data.count);
        fprintf(file, "%d %d\n", state->instructions.count, state->data.count);

        // Assuming file is already opened and valid
        for (int i = 0; i < state->instructions.count; i++) {
            char binary[15];
            char encoded[8];
            intToBinaryString(state->instructions.array[i], binary);
            encodeBase4(binary, encoded);
            printf("Adding line to object file:\n"
                   "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION, encoded);
            fprintf(file, "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION, encoded);
        }

        for (int i = 0; i < state->data.count; i++) {
            char binary[15];
            char encoded[8];
            intToBinaryString(state->data.array[i], binary);
            encodeBase4(binary, encoded);
            printf("Adding line to object file:\n"
                   "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION, encoded);
            fprintf(file, "%04d\t%s\n", i + INDEX_FIRST_INSTRUCTION + state->instructions.count, encoded);
        }

        fclose(file);
        printf("\nObject output file created: %s\n\n", objectFilename);
    } else {
        printf("Failed to create Object output file: %s\n", objectFilename);
    }
    free(objectFilename);
}

void encodeBase4(const char* binary, char* output) {
    char mapping[4] = {'*', '#', '%', '!'};
    for (int i = 0; i < 7; i++) {
        int index = 2 * i;
        int digit = (binary[index] - '0') * 2 + (binary[index + 1] - '0');
        output[i] = mapping[digit];
    }
    output[7] = '\0'; // Null-terminate the string
}