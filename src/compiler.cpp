#include "compiler.h"

// Helper: Generate unique labels
std::string Compiler::newLabel() { 
    return "L" + std::to_string(labelCount++); 
}

// Helper: Manage variable memory addresses
int Compiler::getVarAddress(const char* name) {
    if (varMap.find(name) == varMap.end()) {
        varMap[name] = varCount++;
    }
    return varMap[name];
}

// Main compile function
std::string Compiler::compile(ASTNode* node) {
    asmCode.str(""); // Clear buffer
    varMap.clear();
    varCount = 0;
    traverse(node);
    asmCode << "HALT\n";
    return asmCode.str();
}

// Traversal logic
void Compiler::traverse(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            traverse(node->left); 
            break;
            
        case NODE_VAR_DECL: 
            if (node->left) { 
                traverse(node->left); 
                int addr = getVarAddress(node->data.idName);
                asmCode << "STORE " << addr << "\n";
            }
            break;

        case NODE_ASSIGN: 
            traverse(node->left); 
            asmCode << "STORE " << getVarAddress(node->data.idName) << "\n";
            break;

        case NODE_INT:
            asmCode << "PUSH " << node->data.intValue << "\n";
            break;

        case NODE_VAR: 
            asmCode << "LOAD " << getVarAddress(node->data.idName) << "\n";
            break;

        case NODE_BIN_OP:
            traverse(node->left);
            traverse(node->right);
            switch(node->data.op) {
                case OP_PLUS: asmCode << "ADD\n"; break;
                case OP_MINUS: asmCode << "SUB\n"; break;
                case OP_MULT: asmCode << "MUL\n"; break;
                case OP_DIV: asmCode << "DIV\n"; break;
                case OP_EQ: asmCode << "CMP\n"; break;
                case OP_NEQ: 
                    // CMP returns 1 if a < b. To implement NEQ properly 
                    // requires more complex logic, but for now we can assume
                    // basic subtraction check or similar.
                    // For this lab, simple ops are sufficient.
                    asmCode << "SUB\n"; 
                    break;
                default: break;
            }
            break;
        
        case NODE_IF: {
            std::string lElse = newLabel();
            std::string lEnd = newLabel();
            traverse(node->left); // Condition
            asmCode << "JZ " << lElse << "\n";
            traverse(node->right); // Then
            asmCode << "JMP " << lEnd << "\n";
            asmCode << lElse << ":\n";
            if (node->next) traverse(node->next); // Else
            asmCode << lEnd << ":\n";
            break;
        }
        
        case NODE_WHILE: {
            std::string lStart = newLabel();
            std::string lEnd = newLabel();
            asmCode << lStart << ":\n";
            traverse(node->left); // Condition
            asmCode << "JZ " << lEnd << "\n";
            traverse(node->right); // Body
            asmCode << "JMP " << lStart << "\n";
            asmCode << lEnd << ":\n";
            break;
        }
        default: break;
    }
    
    // Handle next statement in list (Skip next for IF as it uses it for Else)
    if (node->type != NODE_IF && node->next) traverse(node->next);
}