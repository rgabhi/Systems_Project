#ifndef COMPILER_H
#define COMPILER_H

#include "parser/ast.h"
#include <string>
#include <vector>
#include <sstream>
#include <map>

class Compiler {
private:
    std::stringstream asmCode;
    std::map<std::string, int> varMap; 
    int varCount = 0;
    int labelCount = 0;

    std::string newLabel();
    int getVarAddress(const char* name);

public:
    std::string compile(ASTNode* node);
    void traverse(ASTNode* node);
};

#endif