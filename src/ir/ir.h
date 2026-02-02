#ifndef IR_H
#define IR_H

#include "ast.h"

typedef enum {
    OP_LOAD_CONST, // Load literal integer
    OP_LOAD_VAR,   // Load variable value
    OP_STORE_VAR,  // Save to variable
    OP_BINARY_ADD, OP_BINARY_SUB, OP_BINARY_MUL, OP_BINARY_DIV,
    OP_COMPARE_EQ, OP_COMPARE_LT, OP_COMPARE_GT,
    OP_COMPARE_LE , // NEW: For <=
    OP_COMPARE_GE, // NEW: For >=
    OP_COMPARE_NEQ, // NEW
    OP_JUMP,       // Unconditional jump
    OP_JUMP_IF_FALSE,
    OP_HALT,
    OP_ALLOC, //heap action
   

} IROpCode;

typedef struct {
    IROpCode opcode;
    int operand;     // Used for constants or jump offsets
    char* var_name;  // Used for variables
} IRInstruction;

typedef struct {
    IRInstruction* instructions;
    int count;
    int capacity;
} IRProgram;

IRProgram* generate_ir(ASTNode* root);
void print_ir(IRProgram* program);
unsigned char* finalize_bytecode(IRProgram* p, int* out_size) ;

void disassemble_bytecode(unsigned char* bytecode, int length);

#endif