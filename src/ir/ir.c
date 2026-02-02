#include "ir.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
            if(node->data.op == OP_GT){
                compile_ast(node->right, p); //push y first
                compile_ast(node->left, p); // then x
                emit(p, OP_COMPARE_LT, 0, NULL); //check y < x
            }
            else{
                compile_ast(node->left, p);
                compile_ast(node->right, p);
                if (node->data.op == OP_PLUS) emit(p, OP_BINARY_ADD, 0, NULL);
                else if (node->data.op == OP_MINUS) emit(p, OP_BINARY_SUB, 0, NULL);
                else if (node->data.op == OP_MULT) emit(p, OP_BINARY_MUL, 0, NULL);
                else if (node->data.op == OP_DIV) emit(p, OP_BINARY_DIV, 0, NULL);

                // for x < y
                else if(node->data.op == OP_LT) emit(p, OP_COMPARE_LT,0, NULL);
                
            }
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


// --- Updated Helper: Find or add variable to local symbol table ---
int get_var_address(char* name, char** symbol_table, int* symbol_count) {
    for(int i = 0; i < *symbol_count; i++) {
        if(strcmp(symbol_table[i], name) == 0) return i;
    }
    // Not found, add it
    symbol_table[*symbol_count] = strdup(name);
    return (*symbol_count)++;
}

// --- Updated Finalizer ---
unsigned char* finalize_bytecode(IRProgram* p, int* out_size) {
    unsigned char* buffer = (unsigned char*)malloc(1024); 
    int pc = 0;

    // Symbol table for this compilation (Maps variable names -> memory indices)
    char* symbol_table[256]; 
    int symbol_count = 0;

    for (int i = 0; i < p->count; i++) {
        IRInstruction inst = p->instructions[i];
        
        switch (inst.opcode) {
            case OP_LOAD_CONST:
                buffer[pc++] = 0x01; // PUSH
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;

            case OP_LOAD_VAR: { 
                buffer[pc++] = 0x31; // LOAD
                // CORRECTED: Map variable name to dynamic address
                int addr = get_var_address(inst.var_name, symbol_table, &symbol_count);
                memcpy(buffer + pc, &addr, sizeof(int));
                pc += 4;
                break;
            }

            case OP_STORE_VAR: { 
                buffer[pc++] = 0x30; // STORE
                // CORRECTED: Map variable name to dynamic address
                int addr = get_var_address(inst.var_name, symbol_table, &symbol_count);
                memcpy(buffer + pc, &addr, sizeof(int));
                pc += 4;
                break;
            }

            case OP_BINARY_ADD:
                buffer[pc++] = 0x10; // ADD
                break;
            
            case OP_BINARY_SUB:
                buffer[pc++] = 0x11; // SUB (Added missing mapping)
                break;

            case OP_BINARY_MUL:
                buffer[pc++] = 0x12; // MUL (Added missing mapping)
                break;

            case OP_BINARY_DIV:
                buffer[pc++] = 0x13; // DIV (Added missing mapping)
                break;

            case OP_COMPARE_LT:
                buffer[pc++] = 0x14; // CMP
                break;

            case OP_JUMP_IF_FALSE: {
                buffer[pc++] = 0x21; // JZ (Jump if Zero)
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;
            }

            case OP_JUMP: {
                buffer[pc++] = 0x20; // JMP
                memcpy(buffer + pc, &inst.operand, sizeof(int));
                pc += 4;
                break;
            }

            case OP_HALT:
                buffer[pc++] = 0xFF; // HALT
                break;

            default:
                printf("Warning: IR Opcode %d not mapped to BVM bytecode\n", inst.opcode);
                break;
        }
    }
    *out_size = pc;
    
    // Free symbol table strings to avoid leaks in the compiler
    for(int k = 0; k < symbol_count; k++){
        free(symbol_table[k]);
    }

    return buffer;
}