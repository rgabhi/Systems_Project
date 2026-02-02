#ifndef COMMON_H
#define COMMON_H

typedef enum {
    PUSH  = 0x01,
    POP   = 0x02,
    DUP   = 0x03,

    ADD   = 0x10,
    SUB   = 0x11,
    MUL   = 0x12,
    DIV   = 0x13,
    CMP   = 0x14,
    EQ    = 0x15, // NEW: Used for Equality (==)

    JMP   = 0x20,
    JZ    = 0x21,
    JNZ   = 0x22,

    STORE = 0x30,
    LOAD  = 0x31,

    CALL  = 0x40,
    RET   = 0x41,

    HALT  = 0xFF,
    ALLOC = 0x50
} OpCode;



#endif