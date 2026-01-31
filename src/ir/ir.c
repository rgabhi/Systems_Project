#include "ir.h"
#include <string.h>

// Helper to add instructions to the program
void emit(IRProgram* p, IROpCode op, int imm, char* name) {
    if (p->count >= p->capacity) {
        p->capacity *= 2;
        p->instructions = (IRInstruction*)realloc(p->instructions, sizeof(IRInstruction) * p->capacity);
    }
    p->instructions[p->count].opcode = op;
    p->instructions[p->count].operand = imm;
    p->instructions[p->count].var_name = name ? strdup(name) : NULL;
    p->count++;
}

void compile_ast(ASTNode* node, IRProgram* p) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT:
            emit(p, OP_LOAD_CONST, node->data.intValue, NULL);
            break;

        case NODE_VAR:
            emit(p, OP_LOAD_VAR, 0, node->data.idName);
            break;

        case NODE_BIN_OP:
            compile_ast(node->left, p);
            compile_ast(node->right, p);
            if (node->data.op == OP_PLUS) emit(p, OP_BINARY_ADD, 0, NULL);
            else if (node->data.op == OP_MINUS) emit(p, OP_BINARY_SUB, 0, NULL);
            else if (node->data.op == OP_MULT) emit(p, OP_BINARY_MUL, 0, NULL);
            else if (node->data.op == OP_DIV) emit(p, OP_BINARY_DIV, 0, NULL);
            break;

        case NODE_ASSIGN:
        case NODE_VAR_DECL:
            compile_ast(node->left, p); // Evaluate expression
            emit(p, OP_STORE_VAR, 0, node->data.idName);
            break;

        case NODE_BLOCK:
            compile_ast(node->left, p); // Process statement list
            break;

        case NODE_IF: {
            compile_ast(node->left, p); // Condition
            int jump_patch_idx = p->count;
            emit(p, OP_JUMP_IF_FALSE, 0, NULL);
            
            compile_ast(node->right, p); // Then branch
            p->instructions[jump_patch_idx].operand = p->count; // Patch offset
            break;
        }
    }

    if (node->next && node->type != NODE_IF) {
        compile_ast(node->next, p);
    }
}


IRProgram* generate_ir(ASTNode* root) {
    IRProgram* p = (IRProgram*)malloc(sizeof(IRProgram));
    p->capacity = 32;
    p->count = 0;
    p->instructions = (IRInstruction*)malloc(sizeof(IRInstruction) * p->capacity);
    
    compile_ast(root, p);
    emit(p, OP_HALT, 0, NULL);
    return p;
}




// src/ir/ir.c
// src/ir/ir.c
unsigned char* finalize_bytecode(IRProgram* p, int* out_size) {
    unsigned char* buffer = (unsigned char*)malloc(1024); 
    int pc = 0;

    for (int i = 0; i < p->count; i++) {
        IRInstruction inst = p->instructions[i];
        
        switch (inst.opcode) {
            case OP_LOAD_CONST:
                buffer[pc++] = 0x01; // PUSH in commons.h
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;

            case OP_LOAD_VAR: { // Added curly brace for local scope
                buffer[pc++] = 0x31; // LOAD in commons.h
                int idx = 0; // Fixed: now isolated in its own scope
                memcpy(buffer + pc, &idx, sizeof(int));
                pc += 4;
                break;
            }

            case OP_STORE_VAR: { // Added curly brace for local scope
                buffer[pc++] = 0x30; // STORE in commons.h
                int s_idx = 0; // Fixed: now isolated in its own scope
                memcpy(buffer + pc, &s_idx, sizeof(int));
                pc += 4;
                break;
            }

            case OP_BINARY_ADD:
                buffer[pc++] = 0x10; // ADD in commons.h
                break;

            case OP_COMPARE_LT:
                buffer[pc++] = 0x14; // CMP in commons.h
                break;

            case OP_JUMP_IF_FALSE: {
                buffer[pc++] = 0x21; // JZ (Jump if Zero) in commons.h
                // The operand is the byte-offset to jump to
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;
            }

            case OP_JUMP: {
                buffer[pc++] = 0x20; // JMP in commons.h
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;
            }

            case OP_HALT:
                buffer[pc++] = 0xFF; // HALT in commons.h
                break;

            default:
                printf("Warning: IR Opcode %d not mapped to BVM bytecode\n", inst.opcode);
                break;
        }
    }
    *out_size = pc;
    return buffer;
}