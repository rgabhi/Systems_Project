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


        // Now 'registry' is recognized as an external symbol
        if (pid > 0 && pid <= program_count) {
            registry[pid - 1].peak_memory = (long long)vm.st_ptr * sizeof(long long);
            registry[pid - 1].total_allocs = 1; 
            registry[pid - 1].status = TERMINATED;
        }
        
        printf("--- BVM execution complete [Stats Saved] ---\n");
    }
}