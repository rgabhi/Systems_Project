// src/system_manager.h

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "bvm/vm/bvm.h"
#include "bvm/assembler/assembler.h"
#include "compiler.h"
#include <map>
#include <string>

// Ensure C structures and headers are linked correctly
extern "C" {
    #include "parser/ast.h"
    #include "shell/apsh_module.h"
}

struct Process {
    int pid;
    VM* vm;
    std::string source_code;
    bool is_running;
    bool is_debug_mode;
};

class SystemManager {
private:
    std::map<int, Process*> processTable;
    int nextPid = 1;
    Compiler compiler; 

public:
    int submit_program(const char* filename);
    void run_program(int pid);
    void debug_program(int pid);
};

#endif