#ifndef PARSE_FILE_H
#define PARSE_FILE_H
#define _POSIX_C_SOURCE 200809L
#include "Helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 81

void parseFile(AssemblerState* state);

#endif /* PARSE_FILE_H */