#ifndef BVM_H   
#define BVM_H

#define STACK_SIZE 1024
#define MEM_SIZE   256 // memory
#define CODE_SIZE  1024 
#define RET_STACK_SIZE 256

#define LINE_SIZE 128
#define HEAP_SIZE 120000 // define heap


// #include <cstdint>
#include "../gc/gc.h" // GC defs
#include <iostream>
#include <stdlib.h>
#include <cstring>


class VM {
    public:
        // int pc;
        // uint8_t code[CODE_SIZE];
        //program
        unsigned char* program; // ptr to first byte code
        unsigned char* inst_ptr; // instrucion ptr (PC): points to curr instructn
        long long instruction_cnt;
        bool running;

        //stack
        long long stack[STACK_SIZE];
        int st_ptr; // stack ptr

        // memory
        long long memory[MEM_SIZE];


        int ret_stack[RET_STACK_SIZE];
        int rst_ptr;
        int max_sp;
        int total_allocs;
        int total_freed;

        //heap (accessed by GC)
        Object heap[HEAP_SIZE];
        Object* free_list; // to track empty slots

        VM(unsigned char* bytecode);
        void run();
        bool check_stack(int count);
        long long getInstructionCnt();
        void step();
        void inspect_heap_addr(long long addr);

        
};

#endif