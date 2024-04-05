// AddressingTypes.h
#ifndef ADDRESSINGTYPES_H
#define ADDRESSINGTYPES_H

typedef enum {
    IMMEDIATE = 0, DIRECT, CONSTANT_INDEX, DIRECT_REGISTER
} AddressingType;

extern const char* AddressingTypeMethods[];

#endif

