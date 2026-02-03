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
extern ASTNode* parse_program(const char* filename);

typedef enum { SUBMITTED, RUNNING, PAUSED, TERMINATED } ProgramStatus;


typedef struct {
    int pid;               // prog ID 
    char *name;            // prog filename
    ProgramStatus status;  // curr lifecycle state
    ASTNode *ast_root;
    long long peak_memory; // track peak stack usage
    int total_allocs;      // track num of allocations

    long long peak_stack;    
    int current_objects;

    int objects_allocated;   // total objects currently in heap
    int objects_reclaimed;   // objects freed by last GC cycle
    int objects_reachable;
    int object_size;

}ManagedProgram;


#define MAX_PROGRAMS 100
extern ManagedProgram *registry;
extern int program_count;

#include "ir.h"


int apsh_submit(char **args);
int apsh_run(char **args);
int apsh_debug(char **args);
int apsh_kill(char **args);
int apsh_memstat(char **args);
int apsh_gc(char **args);
int apsh_leaks(char **args);

    #endif 