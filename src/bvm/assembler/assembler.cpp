#include "assembler.h"
#include "../commons.h"

// --- Helpers ---
int emit(uint8_t *code, int pc, uint8_t byte) {
    code[pc++] = byte;
    return pc;
}

int emit_int(uint8_t *code, int pc, int value) {
    *(int*)&code[pc] = value;
    return pc + 4;
}

// Helper to trim whitespace from a string
void trim(char *str) {
    if (!str) return;
    
    // Trim leading spaces
    char *start = str;
    while (*start && (*start == ' ' || *start == '\t')) start++;
    
    // Trim trailing spaces
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    *(end + 1) = '\0';
    
    // Move trimmed string to beginning
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// Helper to strip comments and trim
void clean_line(char *line) {
    // Strip comments
    char *comment = strchr(line, ';');
    if (comment) *comment = '\0';
    
    // Trim whitespace
    trim(line);
}

// --- PASS 1: Build Symbol Table ---
int parse_labels(char *line, int pc, std::map<std::string, int> &labels) {
    // Clean the line (remove comments and trim)
    clean_line(line);
    
    // Skip empty lines
    if (strlen(line) == 0) return pc;
    
    char label[64];
    char instr[16];
    char rest[64];
    int val;
    
    // Check for label definition (label:) or (label: INSTRUCTION)
    // Look for a colon in the line
    char *colon_pos = strchr(line, ':');
    if (colon_pos) {
        // Extract label name (everything before the colon)
        int label_len = colon_pos - line;
        if (label_len > 0 && label_len < 64) {
            strncpy(label, line, label_len);
            label[label_len] = '\0';
            trim(label);
            
            // Register the label
            labels[std::string(label)] = pc;
            // printf("DEBUG Pass1: Label '%s' at PC %d\n", label, pc);
            
            // Check if there's an instruction after the colon
            char *after_colon = colon_pos + 1;
            trim(after_colon);
            
            if (strlen(after_colon) == 0) {
                // Label only, no PC increment
                return pc;
            }
            
            // There's an instruction after the label, continue to parse it
            // Copy the instruction part to line for further parsing
            strcpy(line, after_colon);
        }
    }
    
    // Now estimate PC increment based on instruction
    if (sscanf(line, "%s %d", instr, &val) == 2) {
        // Instruction with integer argument (5 bytes: 1 opcode + 4 int)
        return pc + 5;
    }
    else if (sscanf(line, "%s %s", instr, rest) == 2) {
        // Instruction with string argument (assumed to be label, 5 bytes)
        return pc + 5;
    }
    else if (sscanf(line, "%s", instr) == 1) {
        // Instruction with no arguments (1 byte)
        return pc + 1;
    }
    
    return pc;
}

// --- PASS 2: Generate Bytecode ---
int assemble_line(char *line, uint8_t *code, int pc, const std::map<std::string, int> &labels) {
    // Clean the line (remove comments and trim)
    clean_line(line);
    
    // Skip empty lines
    if (strlen(line) == 0) return pc;
    
    char instr[16];
    char arg_str[64];
    int val;
    
    // Skip label-only lines or extract instruction after label
    char *colon_pos = strchr(line, ':');
    if (colon_pos) {
        char *after_colon = colon_pos + 1;
        trim(after_colon);
        
        if (strlen(after_colon) == 0) {
            // Label only, no code generation
            return pc;
        }
        
        // There's an instruction after the label
        strcpy(line, after_colon);
    }
    
    // Parse instruction
    if (sscanf(line, "%s", instr) < 1) return pc;
    
    // CASE 1: Instruction with Integer (e.g., PUSH 5, JMP 30)
    if (sscanf(line, "%s %d", instr, &val) == 2) {
        if (!strcmp(instr, "PUSH")) { pc = emit(code, pc, PUSH); pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JMP"))  { pc = emit(code, pc, JMP);  pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JZ"))   { pc = emit(code, pc, JZ);   pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JNZ"))  { pc = emit(code, pc, JNZ);  pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "LOAD")) { pc = emit(code, pc, LOAD); pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "STORE")){ pc = emit(code, pc, STORE);pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "CALL")) { pc = emit(code, pc, CALL); pc = emit_int(code, pc, val); }
    } 
    // CASE 2: Instruction with Label (e.g., CALL loop, JMP start)
    else if (sscanf(line, "%s %s", instr, arg_str) == 2) {
        if (labels.find(arg_str) == labels.end()) {
            // printf("Error: Undefined label '%s'\n", arg_str);
            exit(1);
        }
        val = labels.at(arg_str);
        // printf("DEBUG Pass2: '%s %s' -> address %d (PC %d)\n", instr, arg_str, val, pc);

        if (!strcmp(instr, "PUSH")) { pc = emit(code, pc, PUSH); pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JMP"))  { pc = emit(code, pc, JMP);  pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JZ"))   { pc = emit(code, pc, JZ);   pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "JNZ"))  { pc = emit(code, pc, JNZ);  pc = emit_int(code, pc, val); }
        else if (!strcmp(instr, "CALL")) { pc = emit(code, pc, CALL); pc = emit_int(code, pc, val); }
    }
    // CASE 3: No Arguments (e.g., ADD, RET, DUP)
    else if (sscanf(line, "%s", instr) == 1) {
        if (!strcmp(instr, "POP")) pc = emit(code, pc, POP);
        else if (!strcmp(instr, "DUP")) pc = emit(code, pc, DUP);
        else if (!strcmp(instr, "ADD")) pc = emit(code, pc, ADD);
        else if (!strcmp(instr, "SUB")) pc = emit(code, pc, SUB);
        else if (!strcmp(instr, "MUL")) pc = emit(code, pc, MUL);
        else if (!strcmp(instr, "DIV")) pc = emit(code, pc, DIV);
        else if (!strcmp(instr, "CMP")) pc = emit(code, pc, CMP);
        else if (!strcmp(instr, "RET")) pc = emit(code, pc, RET);
        else if (!strcmp(instr, "HALT")) pc = emit(code, pc, HALT);
    }

    return pc;
}