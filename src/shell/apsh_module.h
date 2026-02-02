    #ifndef APSH_MODULE_H
    #define APSH_MODULE_H

    // prototypes def
    int apsh_cd(char **args);
    void handle_sigint(int sig);
    void handle_sigchld(int sig);
    int apsh_exit(char **args);
    void execute_pipeline(char **left_args, char **right_args);
    int apsh_export(char **args);
    int check_background(char ** args);
    void add_prompt();


    // struct def
    typedef struct Node {
        char *cmd;
        struct Node *prev;
        struct Node *next;
    } Node;

    typedef struct {
        Node *head;      // Most recently used
        Node *tail;      // Least recently used
        int capacity;    // Max number of commands
        int size;        // Current number of commands
    } LRUCache;

    // Create an LRU history cache
    LRUCache *lru_create(int capacity);
    // Free the LRU cache
    void lru_free(LRUCache *cache);
    // Add a command to the cache (moves existing ones to front)
    void lru_put(LRUCache *cache, const char *cmd);
    // Print history from MRU to LRU
    int lru_print(LRUCache *cache);




typedef struct ASTNode ASTNode; 
extern ASTNode* parse_program(const char* filename); // Your Lab 2 entry function

// --- Integration Assignment Structures ---
typedef enum { SUBMITTED, RUNNING, PAUSED, TERMINATED } ProgramStatus;


typedef struct {
    int pid;               // Custom Program ID 
    char *name;            // Program filename
    ProgramStatus status;  // Current lifecycle state
    ASTNode *ast_root;
    long long peak_memory; // Track peak stack/heap usage
    int total_allocs;      // Track number of allocations

    long long peak_stack;    
    int current_objects;

    // Add these two lines to fix the compilation error
    int objects_allocated;   // Total objects currently in heap
    int objects_reclaimed;   // Objects freed by last GC cycle
    int objects_reachable;

}ManagedProgram;


#define MAX_PROGRAMS 100
extern ManagedProgram *registry;
extern int program_count;

#include "ir.h"


// --- New Command Prototypes ---
int apsh_submit(char **args);
int apsh_run(char **args);
int apsh_debug(char **args);
int apsh_kill(char **args);


// 1. Add prototypes
int apsh_memstat(char **args);
int apsh_gc(char **args);
int apsh_leaks(char **args);

    #endif 