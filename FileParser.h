#ifndef PARSE_FILE_H
#define PARSE_FILE_H
#define _POSIX_C_SOURCE 200809L
#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 256

void ParseFile(AssemblerState* state);
void ReleaseLines(char** lines);

#endif /* PARSE_FILE_H */