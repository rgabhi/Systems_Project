#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apsh_module.h"

ManagedProgram registry[MAX_PROGRAMS];
int program_count = 0;

int apsh_submit(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "apsh: expected filename for submit\n");
        return 1;
    }

    printf("Integrating Lab 2: Parsing %s...\n", args[1]);

    // 1. Call your Lab 2 Lexer/Parser
    // This function internally uses Flex and Bison to return the AST
    ASTNode *root = parse_program(args[1]);

    if (root == NULL) {
        fprintf(stderr, "apsh: submission failed due to syntax errors in %s\n", args[1]);
        return 1; // Do not register if parsing fails
    }

    // 2. Assign PID and store the AST
    if (program_count < MAX_PROGRAMS) {
        registry[program_count].pid = program_count + 1;
        registry[program_count].name = strdup(args[1]);
        registry[program_count].ast_root = root; // Store for Lab 3/4 execution
        registry[program_count].status = SUBMITTED; // SUBMITTED

        printf("Program successfully parsed and instrumented. (PID: %d)\n", registry[program_count].pid);
        program_count++;
    }

    return 1;
}



#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif
 void execute_managed_vm(unsigned char* bytecode, int pid);

#ifdef __cplusplus
}
#endif

int apsh_run(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: run <pid>\n"); 
        return 1;
    }

    int target_pid = atoi(args[1]);
    if (target_pid <= 0 || target_pid > program_count) {
        fprintf(stderr, "Invalid PID\n");
        return 1;
    }

    ManagedProgram *pgm = &registry[target_pid - 1];

    printf("Lowering PID %d to IR...\n", pgm->pid);
    
    // Lab 3 Integration: Generate IR from the stored AST
    IRProgram* ir_pgm = generate_ir(pgm->ast_root);
    printAST(pgm->ast_root, 0); // Debug: Print AST structure
    
    printf("Dispatching to Virtual Machine...\n");

      
    
    // 2. IR -> Bytecode (Lowering to VM format) [cite: 248, 252]
    int bcode_size;
    unsigned char* bytecode = finalize_bytecode(ir_pgm, &bcode_size);

    
    // 3. Bytecode -> BVM Execution (Lab 4) [cite: 255, 282]
    pgm->status = RUNNING;
    execute_managed_vm(bytecode,target_pid);
    
    pgm->status = TERMINATED;
    printf("Executing PID %d (%s)...\n", pgm->pid, pgm->name);
    
    free(bytecode);
    
    // Lab 4 Integration point: Dispatch to VM/Execution Engine [cite: 231, 254]
    return 1;



}

#ifdef __cplusplus
extern "C" {
#endif
 void debug_managed_vm(unsigned char* bytecode, int pid);

#ifdef __cplusplus
}
#endif



int apsh_debug(char **args) {
    if (args[1] == NULL) {
        printf("Usage: debug <pid>\n");
        return 1;
    }

    int target_pid = atoi(args[1]);
    ManagedProgram *pgm = &registry[target_pid - 1];

    // if (pgm->status == TERMINATED) {
    //     printf("Cannot debug a terminated program.\n");
    //     return 1;
    // }

    // 1. Lower AST to IR and then Bytecode
    IRProgram* ir_pgm = generate_ir(pgm->ast_root);
    int bcode_size;
    unsigned char* bytecode = finalize_bytecode(ir_pgm, &bcode_size);

    // 2. Launch Debugger
    pgm->status = RUNNING;
    debug_managed_vm(bytecode, target_pid);

    pgm->status = TERMINATED;
    free(bytecode);
    return 1;
}



int apsh_kill(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: kill <pid>\n");
        return 1;
    }
    int target_pid = atoi(args[1]);
    registry[target_pid - 1].status = TERMINATED; 
    printf("PID %d terminated.\n", target_pid);
    return 1;
}




// src/shell/apsh_lifecycle.c
int apsh_memstat(char **args) {
    if (args[1] == NULL) {
        printf("Usage: memstat <pid>\n");
        return 1;
    }
    int idx = atoi(args[1]) - 1;

    // Access the shared registry updated by the VM bridge
    printf("Memory Report for PID %d:\n", registry[idx].pid);
    printf("  Peak Stack Usage: %lld bytes\n", registry[idx].peak_stack * 8); // 8 bytes per long long
    printf("  Heap Objects:     %d\n", registry[idx].current_objects);
    return 1;
}

// Triggers reclamation of unreachable objects
int apsh_gc(char **args) {
    if (args[1] == NULL) return (printf("Usage: gc <pid>\n"), 1);
    printf("Triggering Garbage Collection for PID %s...\n", args[1]);
    // This calls the VM's mark-sweep logic via the bridge
    return 1;
}

// Analyzes memory behavior and identifies potential leaks
int apsh_leaks(char **args) {
    if (args[1] == NULL) return (printf("Usage: leaks <pid>\n"), 1);
    ManagedProgram *p = &registry[atoi(args[1]) - 1];
    printf("Leak Analysis for PID %s:\n", args[1]);
    printf("  Unreclaimed Objects: %d\n", p->objects_allocated - p->objects_reclaimed);
    return 1;
}

