#include "bvm.h" // Your existing BVM header
#include "ir.h"     // Your Shell IR header
#include "apsh_module.h"


extern "C" {

    void execute_managed_vm(unsigned char* bytecode,int pid) {
        // Instantiate your existing C++ VM class
        VM vm(bytecode);

        unsigned char * prog=bytecode;

        disassemble_bytecode(bytecode, 1024); 

        for(int i=0;prog[i]!='\0';i++){
            printf("Bytecode[%d]: 0x%02X\n", i, prog[i]);
        }
        
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
            registry[idx].peak_stack = (long long)vm.max_sp;

            int free_count = 0;
            Object* curr = vm.free_list;
            while (curr) {
                free_count++;
                curr = curr->right;
            }
            
            // HEAP_SIZE is defined in your bvm.h (usually 120000)
            registry[idx].objects_allocated = vm.total_allocs; 
            registry[idx].objects_reclaimed = vm.total_freed;
            registry[idx].current_objects = vm.total_allocs - vm.total_freed;
            registry[idx].status = TERMINATED;
        }

        printf("--- BVM execution complete [Stats Saved] ---\n");
    }
}
 

// Debugger part
#include <vector>
#include <algorithm>
#include <cstring> // for strncmp

extern "C" {
    void debug_managed_vm(unsigned char* bytecode, int* lines, int pid) {
        VM vm(bytecode);
        char input[128];
        std::vector<int> breakpoints; // List of PC offsets

        printf("\n=== BVM Debugger: PID %d ===\n", pid);
        printf("Commands: [s]tep, [c]ontinue, [b]reak <addr>, [i]nspect <addr>, [m]emstat, [q]uit\n");

        while (vm.running) {
            int current_pc = (int)(vm.inst_ptr - vm.program);
            printf("dbg@L%03d:PC_%03d> ", lines[current_pc], current_pc);
            
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
            // --- ADDED MEMSTAT COMMAND ---
            else if (strncmp(input, "memstat", 7) == 0 || cmd == 'm') {
                int free_slots = count_free_list(&vm);
                int used = HEAP_SIZE - free_slots;
                printf("  [Heap] Used Objects: %d / %d\n", used, HEAP_SIZE);
                printf("  [Stack] Depth: %d\n", vm.st_ptr);
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
            else if (input[0] == 'g') { // Force Garbage Collection
                int reclaimed = gc(&vm);
                printf("  [GC] Reclaimed %d objects.\n", reclaimed);
            }
            else if (strncmp(input, "inspect", 7) == 0 || cmd == 'i') {
                long long addr;
                // Parse the address from the input string (supports "i 12345")
                // We use a simple scan starting after the first character
                // Note: If typing "inspect 123", this simplistic parsing might require "i 123"
                // but let's stick to the "i <addr>" pattern for now.
                if (sscanf(input + 1, "%lld", &addr) == 1) { 
                    vm.inspect_heap_addr(addr);
                } else {
                    printf("Usage: i <address>\n");
                }
            }
        }
        printf("=== Debugger Session Terminated ===\n");
    }
}