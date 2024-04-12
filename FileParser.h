#ifndef PARSE_FILE_H
#define PARSE_FILE_H

typedef struct {
    char** lines;
    int numberOfLines;
} ParsedFile;

ParsedFile ParseFile(const char* filename);

void ReleaseLines(char** lines);

#endif /* PARSE_FILE_H */