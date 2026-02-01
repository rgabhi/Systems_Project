#include "bvm.h" // Your existing BVM header
#include "ir.h"     // Your Shell IR header
#include "apsh_module.h"


extern "C" {
    void execute_managed_vm(unsigned char* bytecode,int pid) {
        // Instantiate your existing C++ VM class
        VM vm(bytecode);
        
        printf("--- BVM managed execution starting ---\n");
        vm.run(); // Call your existing run loop
        
       if(vm.st_ptr > 0) {   
            printf("Final VM stack top: %lld\n", (long long)vm.stack[vm.st_ptr - 1]);
        }else{
            printf("Stack: [Empty]\n");
            printf("Memory[0] %d\n", vm.memory[0]);
        }
        // Capture data for Lab 5 BEFORE the vm object is destroyed
        if (pid > 0 && pid <= MAX_PROGRAMS) {
        int idx = pid - 1;
       registry[idx].peak_stack = (long long)vm.st_ptr;

        int free_count = 0;
        Object* curr = vm.free_list;
        while (curr) {
            free_count++;
            curr = curr->right;
        }
        
        // HEAP_SIZE is 120,000 in your bvm.h
        registry[idx].objects_allocated = 120000 - free_count; 
        registry[idx].current_objects = registry[idx].objects_allocated;
        registry[idx].status = TERMINATED;
           
        }

        if(vm.st_ptr > 0) {   
            printf("Final VM stack top: %lld\n", (long long)vm.stack[vm.st_ptr - 1]);
        } 
            printf("Stack: [Empty]\nMemory[0] %d\n", vm.memory[0]);
        
        printf("--- BVM execution complete [Stats Saved] ---\n");
    }
}
 

//Debugger part
#include <vector>
#include <algorithm>

extern "C" {
    void debug_managed_vm(unsigned char* bytecode, int pid) {
        VM vm(bytecode);
        char input[128];
        std::vector<int> breakpoints; // List of PC offsets

        printf("\n=== BVM Debugger: PID %d ===\n", pid);
        printf("Commands: [s]tep, [c]ontinue, [b]reak <addr>, [r]egs, [q]uit\n");

        while (vm.running) {
            int current_pc = (int)(vm.inst_ptr - vm.program);
            printf("dbg@PC_%03d> ", current_pc);
            
            if (!fgets(input, sizeof(input), stdin)) break;
            char cmd = input[0];

            if (cmd == 'q') break;
            
            if (cmd == 'b') { // Set Breakpoint
                int addr;
                if (sscanf(input + 2, "%d", &addr) == 1) {
                    breakpoints.push_back(addr);
                    printf("Breakpoint set at byte offset %d\n", addr);
                }
            } 
            else if (cmd == 's') { // Single Step
                vm.step();
                if (vm.st_ptr > 0) printf("  Stack Top: %lld\n", vm.stack[vm.st_ptr-1]);
            } 
            else if (cmd == 'r') { // Inspect Registers
                printf("  PC: %d | SP: %d | Inst Count: %lld\n", 
                       current_pc, vm.st_ptr, vm.instruction_cnt);
            } 
            else if (cmd == 'c') { // Continue until breakpoint or halt
                vm.step(); // Step once to move past current breakpoint
                while (vm.running) {
                    current_pc = (int)(vm.inst_ptr - vm.program);
                    if (std::find(breakpoints.begin(), breakpoints.end(), current_pc) != breakpoints.end()) {
                        printf("Hit breakpoint at PC %d\n", current_pc);
                        break;
                    }
                    vm.step();
                }
            }
        }
        printf("=== Debugger Session Terminated ===\n");
    }
}