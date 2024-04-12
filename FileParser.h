#ifndef PARSE_FILE_H
#define PARSE_FILE_H
#include "helper.h"
#define MAX_LINE_LENGTH 256

ParsedFile ParseFile(const char* filename);

void ReleaseLines(char** lines);

#endif /* PARSE_FILE_H */