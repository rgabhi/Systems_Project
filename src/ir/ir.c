#include "ir.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Helper to add instructions to the program
void emit(IRProgram* p, IROpCode op, int imm, char* name, int lineNumber) {
    if (p->count >= p->capacity) {
        p->capacity *= 2;
        p->instructions = (IRInstruction*)realloc(p->instructions, sizeof(IRInstruction) * p->capacity);
    }
    p->instructions[p->count].opcode = op;
    p->instructions[p->count].operand = imm;
    p->instructions[p->count].var_name = name ? strdup(name) : NULL;
    p->instructions[p->count].lineNumber = lineNumber;
    p->count++;
}

void compile_ast(ASTNode* node, IRProgram* p) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT:
            emit(p, OP_LOAD_CONST, node->data.intValue, NULL, node->lineNumber);
            break;

        case NODE_VAR:
            emit(p, OP_LOAD_VAR, 0, node->data.idName, node->lineNumber);
            break;

        case NODE_BIN_OP:
            if(node->data.op == OP_GT){
                compile_ast(node->right, p); 
                compile_ast(node->left, p); 
                emit(p, OP_COMPARE_LT, 0, NULL, node->lineNumber); 
            }
            else if(node->data.op == OP_GE){
                compile_ast(node->right, p); compile_ast(node->left, p); 
                emit(p, OP_COMPARE_LE, 0, NULL, node->lineNumber); 
            }
            else{
                compile_ast(node->left, p);
                compile_ast(node->right, p);
                if (node->data.op == OP_PLUS) emit(p, OP_BINARY_ADD, 0, NULL, node->lineNumber);
                else if (node->data.op == OP_MINUS) emit(p, OP_BINARY_SUB, 0, NULL, node->lineNumber);
                else if (node->data.op == OP_MULT) emit(p, OP_BINARY_MUL, 0, NULL, node->lineNumber);
                else if (node->data.op == OP_DIV) emit(p, OP_BINARY_DIV, 0, NULL, node->lineNumber);
                else if (node->data.op == OP_LT) emit(p, OP_COMPARE_LT, 0, NULL, node->lineNumber);
                else if (node->data.op == OP_EQ) emit(p, OP_COMPARE_EQ, 0, NULL, node->lineNumber);
                // FIX: Handle <=
                else if (node->data.op == OP_LE) emit(p, OP_COMPARE_LE, 0, NULL, node->lineNumber);
                // FIX: Use dedicated GE opcode
                else if (node->data.op == OP_GE) emit(p, OP_COMPARE_GE, 0, NULL, node->lineNumber);

                // FIX: Handle !=
            else if (node->data.op == OP_NEQ) emit(p, OP_COMPARE_NEQ, 0, NULL, node->lineNumber);

            else if (node->data.op == OP_NEQ) emit(p, OP_COMPARE_NEQ, 0, NULL, node->lineNumber);
            
            }
            break;

        case NODE_ASSIGN:
        case NODE_VAR_DECL:
            compile_ast(node->left, p); 
            emit(p, OP_STORE_VAR, 0, node->data.idName, node->lineNumber);
            break;

        case NODE_BLOCK:
            compile_ast(node->left, p); 
            break;

        case NODE_IF: {
            compile_ast(node->left, p); // Condition
            
            int jump_to_else_idx = p->count;
            emit(p, OP_JUMP_IF_FALSE, 0, NULL, node->lineNumber);
            
            compile_ast(node->right, p); // Then
            
            int jump_to_end_idx = -1;
            if (node->next) {
                 jump_to_end_idx = p->count;
                 emit(p, OP_JUMP, 0, NULL, node->lineNumber);
            }

            p->instructions[jump_to_else_idx].operand = p->count; 
            
            if (node->next) {
                compile_ast(node->next, p);
                if(jump_to_end_idx != -1) {
                    p->instructions[jump_to_end_idx].operand = p->count;
                }
            }
            break;
        }
        
       case NODE_WHILE: {
            int start_idx = p->count;
            compile_ast(node->left, p);
            int jump_out_idx = p->count;
            emit(p, OP_JUMP_IF_FALSE, 0, NULL, node->lineNumber);
            compile_ast(node->right, p);
            emit(p, OP_JUMP, start_idx, NULL, node->lineNumber);
            p->instructions[jump_out_idx].operand = p->count;
            break;
        }
        case NODE_HEAP_ALLOC:{
            compile_ast(node->left, p);
            emit(p, OP_ALLOC, 0, NULL, node->lineNumber);
            break;
        }
        case NODE_UNARY:
            if (node->data.op == OP_NEG) { // Assuming OP_NEG is '-'
                // To do -X, we can do 0 - X
                emit(p, OP_LOAD_CONST, 0, NULL, node->lineNumber); // Push 0
                compile_ast(node->left, p);      // Push X
                emit(p, OP_BINARY_SUB, 0, NULL, node->lineNumber); // 0 - X
            }
                 // NEW: Handle @
           else if (node->data.op == OP_DEREF) {
                // Handle Dereference (@x or *x)
                compile_ast(node->left, p);      // 1. Evaluate the expression (get the address)
                emit(p, OP_IR_DEREF, 0, NULL, node->lineNumber);   // 2. Emit the DEREF instruction
            }
            break;
    }

    // --- CRITICAL FIX: STOP DUPLICATE TRAVERSAL ---
    // Do NOT visit 'next' if we are an IF node, because the parser 
    // likely put the 'else' block into 'next' as well.
    if (node->next && node->type != NODE_IF) {
        compile_ast(node->next, p);
    }
}

int get_var_address(char* name, char** symbol_table, int* symbol_count) {
    for(int i = 0; i < *symbol_count; i++) {
        if(strcmp(symbol_table[i], name) == 0) return i;
    }
    symbol_table[*symbol_count] = strdup(name);
    return (*symbol_count)++;
}

IRProgram* generate_ir(ASTNode* root) {
    IRProgram* p = (IRProgram*)malloc(sizeof(IRProgram));
    p->capacity = 32;
    p->count = 0;
    p->instructions = (IRInstruction*)malloc(sizeof(IRInstruction) * p->capacity);
    compile_ast(root, p);
    emit(p, OP_HALT, 0, NULL, root->lineNumber);
    return p;
}

unsigned char* finalize_bytecode(IRProgram* p, int* out_size, int** out_lines) {
    unsigned char* buffer = (unsigned char*)malloc(2048); 
    *out_lines = (int*)malloc(2048 * sizeof(int)); // Allocate line map
    int pc = 0;
    char* symbol_table[256]; 
    int symbol_count = 0;

    // PASS 1: Calculate Byte Offsets
    int* offsets = (int*)malloc(sizeof(int) * (p->count + 1));
    int current_offset = 0;
    
    for (int i = 0; i < p->count; i++) {
        offsets[i] = current_offset;
        IROpCode op = p->instructions[i].opcode;
        switch(op) {
            case OP_LOAD_CONST: case OP_LOAD_VAR: case OP_STORE_VAR:
            case OP_JUMP: case OP_JUMP_IF_FALSE:
                current_offset += 5; break;
            default:
                current_offset += 1; break;
        }
    }
    offsets[p->count] = current_offset; 

    // PASS 2: Emit Code
    for (int i = 0; i < p->count; i++) {
        IRInstruction inst = p->instructions[i];
        int start_pc = pc;
        
        switch (inst.opcode) {
            case OP_LOAD_CONST:
                buffer[pc++] = 0x01; 
                memcpy(buffer + pc, &inst.operand, sizeof(int)); pc += 4; break;
            case OP_LOAD_VAR: { 
                buffer[pc++] = 0x31; 
                int addr = get_var_address(inst.var_name, symbol_table, &symbol_count);
                memcpy(buffer + pc, &addr, sizeof(int)); pc += 4; break;
            }
            case OP_STORE_VAR: { 
                buffer[pc++] = 0x30; 
                int addr = get_var_address(inst.var_name, symbol_table, &symbol_count);
                memcpy(buffer + pc, &addr, sizeof(int)); pc += 4; break;
            }
            case OP_BINARY_ADD: buffer[pc++] = 0x10; break;
            
            case OP_COMPARE_EQ: buffer[pc++] = 0x15; break;

            case OP_COMPARE_LE: buffer[pc++] = 0x16; break; // Map to 0x16
            case OP_COMPARE_GE: buffer[pc++] = 0x17; break; // MAP TO 0x17
            case OP_COMPARE_NEQ: buffer[pc++] = 0x18; break; // MAP TO 0x18
            case OP_UNARY_NEG:  buffer[pc++] = 0x19; break; // MAP TO 0x19
            
            case OP_IR_DEREF: buffer[pc++] = 0x1E; break; // Map to 0x1E
            
            
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
            
            case OP_ALLOC:
                buffer[pc++] = 0x50; // ALLOC
                break;

            case OP_JUMP_IF_FALSE: {
                buffer[pc++] = 0x21; 
                int t = offsets[inst.operand];
                memcpy(buffer + pc, &t, sizeof(int)); pc += 4; break;
            }
            case OP_JUMP: {
                buffer[pc++] = 0x20; 
                int t = offsets[inst.operand];
                memcpy(buffer + pc, &t, sizeof(int)); pc += 4; break;
            }
            case OP_HALT: buffer[pc++] = 0xFF; break;
            default: buffer[pc++] = 0x00; break;
        }
        for (int k = start_pc; k < pc; k++) {
            (*out_lines)[k] = inst.lineNumber;
        }
    }
    *out_size = pc;
    free(offsets);
    for(int k = 0; k < symbol_count; k++) free(symbol_table[k]);
    return buffer;
}


//Just for printgin purposes  


void disassemble_bytecode(unsigned char* bytecode, int length) {
    printf("\n=== BYTECODE DISASSEMBLY ===\n");
    int pc = 0;
    
    while (pc < length) {
        unsigned char opcode = bytecode[pc];
        printf("0x%04X:  ", pc); // Print Byte Offset

        switch (opcode) {
            // --- 5-Byte Instructions (Opcode + 4-Byte Operand) ---
            case 0x01: { // PUSH
                int val;
                memcpy(&val, &bytecode[pc + 1], sizeof(int));
                printf("PUSH %d\n", val);
                pc += 5;
                break;
            }
            case 0x30: { // STORE
                int addr;
                memcpy(&addr, &bytecode[pc + 1], sizeof(int));
                printf("STORE [Addr %d]\n", addr);
                pc += 5;
                break;
            }
            case 0x31: { // LOAD
                int addr;
                memcpy(&addr, &bytecode[pc + 1], sizeof(int));
                printf("LOAD [Addr %d]\n", addr);
                pc += 5;
                break;
            }
            case 0x20: { // JMP
                int target;
                memcpy(&target, &bytecode[pc + 1], sizeof(int));
                printf("JMP @ 0x%04X\n", target);
                pc += 5;
                break;
            }
            case 0x21: { // JZ
                int target;
                memcpy(&target, &bytecode[pc + 1], sizeof(int));
                printf("JZ @ 0x%04X\n", target);
                pc += 5;
                break;
            }
            case 0x22: { // JNZ
                int target;
                memcpy(&target, &bytecode[pc + 1], sizeof(int));
                printf("JNZ @ 0x%04X\n", target);
                pc += 5;
                break;
            }
            case 0x40: { // CALL
                int target;
                memcpy(&target, &bytecode[pc + 1], sizeof(int));
                printf("CALL @ 0x%04X\n", target);
                pc += 5;
                break;
            }
            case 0x17: printf("GE (>=)\n"); pc++; break; // Added Display
            case 0x18: printf("NEQ (!=)\n"); pc++; break; // Display

            // --- 1-Byte Instructions ---
            case 0x02: printf("POP\n"); pc++; break;
            case 0x03: printf("DUP\n"); pc++; break;
            case 0x10: printf("ADD\n"); pc++; break;
            case 0x11: printf("SUB\n"); pc++; break;
            case 0x12: printf("MUL\n"); pc++; break;
            case 0x13: printf("DIV\n"); pc++; break;
            case 0x19: printf("NEG\n"); pc++; break; // Added Display

           
            case 0x41: printf("RET\n"); pc++; break;
            // Inside disassemble_bytecode function switch(opcode):

            case 0x14: printf("CMP (LT)\n"); pc++; break;
            case 0x15: printf("EQ\n"); pc++; break; // <--- ADD THIS

            case 0x16: printf("LE (<=)\n"); pc++; break;

            case 0x1E: printf("DEREF (@)\n"); pc++; break;
          
            
            case 0xFF: 
                printf("HALT\n"); 
                pc++; 
                return; // Stop printing at HALT

            default:
                printf("UNKNOWN (0x%02X)\n", opcode);
                pc++;
                break;
        }
    }
    printf("============================\n\n");
}