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

void execute_managed_vm(unsigned char* bytecode);

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
    
    printf("Dispatching to Virtual Machine...\n");

      
    
    // 2. IR -> Bytecode (Lowering to VM format) [cite: 248, 252]
    int bcode_size;
    unsigned char* bytecode = finalize_bytecode(ir_pgm, &bcode_size);
    
    // 3. Bytecode -> BVM Execution (Lab 4) [cite: 255, 282]
    pgm->status = RUNNING;
    execute_managed_vm(bytecode);
    
    pgm->status = TERMINATED;
    printf("Executing PID %d (%s)...\n", pgm->pid, pgm->name);
    free(bytecode);
    
    // Lab 4 Integration point: Dispatch to VM/Execution Engine [cite: 231, 254]
    return 1;



}

int apsh_debug(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: debug <pid>\n");
        return 1;
    }
    // Lab 4 Integration point: Enter instruction-level stepping mode [cite: 257, 260]
    printf("Entering debug mode for PID %s\n", args[1]);
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

int apsh_memstat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Usage: memstat <pid>\n");
        return 1;
    }

    int target_pid = atoi(args[1]);
    if (target_pid <= 0 || target_pid > program_count) {
        fprintf(stderr, "Invalid PID\n");
        return 1;
    }

    ManagedProgram *pgm = &registry[target_pid - 1];
    printf("\n--- Memory Statistics for PID %d (%s) ---\n", pgm->pid, pgm->name);
    printf("Program Status: %s\n", 
           pgm->status == SUBMITTED ? "SUBMITTED" : 
           pgm->status == RUNNING ? "RUNNING" : "TERMINATED");
    
    // These values are updated by the VM during execution
    printf("Peak Stack Usage: %lld bytes\n", pgm->peak_memory);
    printf("Dynamic Objects:  %d\n", pgm->total_allocs);
    printf("------------------------------------------\n");

    return 1;
}