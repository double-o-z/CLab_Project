#include "FirstPass.h"

/*  Main function to start the first pass */
void firstPass(AssemblerState* state) {
    int i;

    if (state->debugMode){
        printf("First pass for: %s\n", state->inputFilename);
    }

    for (state->parsedFile.currentLineNum = 0;
         state->parsedFile.currentLineNum < state->parsedFile.numberOfLines;
         state->parsedFile.currentLineNum++) {
        /*  Pass the current line number, i, to processLine; line numbers start at 1 for user clarity */
        processLine(state);
    }

    /*  Update symbols table with this indices change. */
    for (i = 0; i < state->symbolsCount; ++i) {
        if (state->symbols[i].type == DATA || state->symbols[i].type == ENTRY){
            state->symbols[i].value += state->instructions.count + INDEX_FIRST_INSTRUCTION;
        } else if (state->symbols[i].type == CODE){
            state->symbols[i].value += INDEX_FIRST_INSTRUCTION;
        }
    }

    /*  Call helper functions to print symbols table and data list */
    if (state->debugMode){
        printSymbolsTable(state);
        printDataList(state);
        printInstructionsList(state);
    }
}

/*  Process a line to determine its type and handle labels */
void processLine(AssemblerState* state) {
    char *line, **parts, *label, *command, *operands;
    int len;

    if (state->debugMode){
        printf("Handling line number %d, with text: %s\n",
               state->parsedFile.currentLineNum + 1,
               state->parsedFile.lines[state->parsedFile.currentLineNum]);
    }

    line = state->parsedFile.lines[state->parsedFile.currentLineNum];
    /*  Skip empty lines and lines starting with ';' */
    if (line[0] == '\0' || line[0] == ';') {
        return;
    }

    parts = splitFirstWhitespace(line);
    label = NULL;
    command = parts[0];
    operands = parts[1];

    /*  Check if the line defines a label */
    len = strlen(command);
    if (command[len - 1] == ':') {
        command[len - 1] = '\0';  /*  Remove colon */
        label = command;
        if (operands) {
            parts = splitFirstWhitespace(operands);
            command = parts[0];
            operands = parts[1];
        }
    }

    if (label != NULL && !isValidSymbol(label)){
        printf("Error on line %d: Invalid command label '%s'\n",
               state->parsedFile.currentLineNum + 1,
               label);
        state->assemblerError = true;
    } else {
        if (state->debugMode){
            printf("handling command %s, operands: %s, and label: %s\n", command, operands, label);
        }

        /*  Handle command type determination here (to be implemented) */
        /*  For example, determine if it's a data directive or an instruction */
        /*  Handle .define directive */
        if (strcmp(command, ".define") == 0) {
            handleDefineDirective(state, operands);
        } else if (strcmp(command, ".data") == 0) {
            handleDataDirective(state, operands, label);
        } else if (strcmp(command, ".string") == 0) {
            handleStringDirective(state, operands, label);
        } else if (strcmp(command, ".extern") == 0) {
            handleExternalDirective(state, operands);
        } else if (strcmp(command, ".entry") == 0) {
            if (state->debugMode){
                printf("Found entry, do nothing in first pass.\n");
            }
        } else {
            /*  If it's not a directive, treat it as an instruction */
            handleInstructions(state, label, command, operands);
        }
    }

    free(parts);  /*  Free parts array after use */
}

void handleInstructions(AssemblerState* state, char* label, char* command, char* operands) {
    Symbol newSymbol;
    int firstWord, srcType, destType, opcodeIndex;


    /*  If a label is present, add it to the symbol table with the current instruction count (IC) */
    if (label) {
        newSymbol.label = strdup(label);
        newSymbol.type = CODE;
        newSymbol.value = state->instructions.count;
        dynamicInsertSymbol(state, newSymbol);
    }

    /*  Find opcode index based on the command */
    opcodeIndex = findOpcodeIndex(command);  /*  Assuming this function exists */
    if (opcodeIndex == -1) {
        printf("Error on line %d: Unknown command '%s'\n"
               "with line: %s\n",
               state->parsedFile.currentLineNum + 1,
               command,
               state->parsedFile.lines[state->parsedFile.currentLineNum]);
        state->assemblerError = true;
        return;
    }
    /*  Validate and parse operands */
    srcType = -1;
    destType = -1;
    if (!parseOperands(state, &srcType, &destType, operands, opcodes[opcodeIndex])) {
        printf("Error on line %d: Invalid operands for '%s'\n", state->parsedFile.currentLineNum + 1, command);
        state->assemblerError = true;
        return;
    }
    /*  Create the first word of the instruction */
    firstWord = createFirstWord(state, srcType, destType, opcodes[opcodeIndex].opcode);
    if (state->debugMode){
        printf("adding first instruction word with value: %d\n", firstWord);
    }

    addInstructionToInstructionList(state, &state->instructions, firstWord);

    /*  Prepare for additional words that describe operand data (handled during the second pass) */
    prepareOperandDataWords(state, srcType, destType);
}

/*  Function to handle the .define directive */
void handleDefineDirective(AssemblerState* state, char* operands) {
    Symbol newSymbol;
    int lineNumber;
    char *fullLine, *equalSign, *name, *valueStr;

    lineNumber = state->parsedFile.currentLineNum + 1;
    fullLine = state->parsedFile.lines[lineNumber-1];

    if (!operands) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        state->assemblerError = true;
        return;
    }

    operands = trim(operands);
    equalSign = strchr(operands, '=');
    if (!equalSign) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        state->assemblerError = true;
        return;
    }

    *equalSign = '\0';
    name = trim(operands);
    valueStr = trim(equalSign + 1);

    if (!isalpha(name[0]) || !isValidInteger(valueStr)) {
        fprintf(stderr, "Error in line: %d, invalid .define command: %s\n", lineNumber, fullLine);
        state->assemblerError = true;
        return;
    }

    newSymbol.type = MDEFINE;
    newSymbol.label = strdup(name);
    newSymbol.value = atoi(valueStr);
    dynamicInsertSymbol(state, newSymbol);
}

/*  Function to handle the .data directive */
void handleDataDirective(AssemblerState* state, char* operands, const char* label) {
    Symbol newSymbol;
    int lineNumber, countBeforeInsert, value;
    char *line, *token;

    lineNumber = state->parsedFile.currentLineNum + 1;
    line = state->parsedFile.lines[lineNumber-1];

    if (!label || label[0] == '\0') {
        fprintf(stderr,
                "Error in line: %d, .data command requires a label: %s\n", lineNumber, line);
        state->assemblerError = true;
        return;
    }

    if (!operands) {
        fprintf(stderr,
                "Error in line: %d, invalid .data command, no data provided: %s\n", lineNumber, line);
        state->assemblerError = true;
        return;
    }

    countBeforeInsert = state->data.count;
    token = strtok(operands, ",");
    while (token) {
        token = trim(token);
        if (isValidInteger(token)) {
            value = atoi(token); /*  Convert to int */
        } else {
            value = findSymbolValue(state, token);
            if (value == -1) {
                fprintf(stderr,
                        "Error in line: %d, invalid operand or undefined symbol used in .data: %s\n",
                        lineNumber, token);
                state->assemblerError = true;
                token = strtok(NULL, ",");
                continue;
            }
        }
        dynamicInsert(state, &state->data, value);
        token = strtok(NULL, ",");
    }

    newSymbol.type = DATA;
    newSymbol.label = strdup(label);
    newSymbol.value = countBeforeInsert;
    dynamicInsertSymbol(state, newSymbol);
}

/*  Function to handle the .data directive */
void handleExternalDirective(AssemblerState* state, char* operands) {
    Symbol newSymbol, *symbol;
    int lineNumber;
    char *line, *token;

    lineNumber = state->parsedFile.currentLineNum + 1;
    line = state->parsedFile.lines[lineNumber-1];

    if (!operands) {
        fprintf(stderr,
                "Error in line: %d, invalid .extern command, no data provided: %s\n", lineNumber, line);
        state->assemblerError = true;
        return;
    }

    token = strtok(operands, ",");
    while (token) {
        token = trim(token);
        symbol = findSymbolInST(state, token);
        if (symbol != NULL) {
            fprintf(stderr, "Error in line: %d, invalid operand for .extern command, "
                            "it is already defined locally: %s\n", lineNumber, token);
            state->assemblerError = true;
            token = strtok(NULL, ",");
            continue;
        }

        newSymbol.type = EXTERNAL;
        newSymbol.label = strdup(token);
        newSymbol.value = 0;
        dynamicInsertSymbol(state, newSymbol);
        token = strtok(NULL, ",");
    }
}

/*  Function to handle the .string directive */
void handleStringDirective(AssemblerState* state, char* operands, const char* label) {
    Symbol newSymbol;
    int lineNumber, countBeforeInsert;
    char *line, *character;

    lineNumber = state->parsedFile.currentLineNum + 1;
    line = state->parsedFile.lines[lineNumber-1];

    if (!label || label[0] == '\0') {
        fprintf(stderr, "Error in line: %d, .string command requires a label: %s\n", lineNumber, line);
        state->assemblerError = true;
        return;
    }

    if (!operands || operands[0] != '"' || operands[strlen(operands) - 1] != '"') {
        fprintf(stderr,
                "Error in line: %d, invalid .string command, string must be enclosed in double quotes: %s\n",
                lineNumber, line);
        state->assemblerError = true;
        return;
    }

    countBeforeInsert = state->data.count;
    operands++;  /*  Skip the initial double quote */
    operands[strlen(operands) - 1] = '\0'; /*  Remove the closing double quote */
    for (character = operands; *character; character++) { /*  For each char in the string. */
        if ((unsigned char)*character > 127) {  /*  if not a letter from a-z,A-z, give error. */
            fprintf(stderr,
                    "Error in line: %d, non-ASCII character found in .string: %s\n",
                    lineNumber, line);
            state->assemblerError = true;
            return;
        }
        dynamicInsert(state, &state->data, (int)*character);  /*  Add the letter to data list. */
    }

    dynamicInsert(state, &state->data, 0); /*  Add terminating zero */
    newSymbol.type = DATA;
    newSymbol.label = strdup(label);
    newSymbol.value = countBeforeInsert;
    dynamicInsertSymbol(state, newSymbol); /*  Insert the label to symbols table. */
}
