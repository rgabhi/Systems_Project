#include "gc.h"
#include "../vm/bvm.h" // Include full VM definition to access stack/heap
#include <stdio.h>



// --- Helper func ---

// mark_obj
void mark_object(Object* obj) {
    if (obj == NULL || obj->marked) {
        return;
    }
    
    obj->marked = true;
    
    if(obj->type == OBJ_PAIR){
        // recursively mark
        mark_object(obj->left);
        mark_object(obj->right);
    } 
}

// --- wrappers ---

// HEAP
// new_int
Object* new_int(VM* vm, int value) {
    if (vm->free_list == NULL) {
        // 1. Attempt to reclaim memory
        gc(vm); 

        // 2. Check again: Did GC actually give us any space?
        if (vm->free_list == NULL) {
            printf("Heap Overflow\n");
            return NULL;
        }
    }

    Object* curr = vm->free_list;
    vm->total_allocs++;
    vm->free_list = vm->free_list->right;

    curr->type = OBJ_INT;
    curr->value = value;
    curr->left = NULL;
    curr->right = NULL;
    curr->marked = false;

    return curr;
}


// new_pair
Object* new_pair(VM* vm, Object* l, Object* r) {
    if (vm->free_list == NULL) {
        // 1. Attempt to reclaim memory
        gc(vm); 

        // 2. Check again: Did GC actually give us any space?
        if (vm->free_list == NULL) {
            printf("Heap Overflow\n");
            return NULL;
        }
    }

    Object* curr = vm->free_list;
    vm->total_allocs++;
    vm->free_list = vm->free_list->right;

    curr->type = OBJ_PAIR;
    curr->left = l;
    curr->right = r;
    curr->marked = false;

    return curr;
}

Object* new_function(VM* vm) {
    return new_pair(vm, NULL, NULL);
}

Object* new_closure(VM* vm, Object* fn, Object* env) {
    return new_pair(vm, fn, env);
}

void push(VM* vm, Object* obj) {
    if (vm->st_ptr >= STACK_SIZE) {
        printf("Stack Overflow\n");
        return;
    }
    // store ptr as integer on the stack
    vm->stack[vm->st_ptr] = (long long)obj;
    vm->st_ptr++;
}

Object* pop(VM* vm) {
    if (vm->st_ptr <= 0) {
        printf("Stack Underflow\n");
        return NULL;
    }
    vm->st_ptr--;
    return (Object*)vm->stack[vm->st_ptr];
}

// --- helper to count free list size ---
int count_free_list(VM* vm) {
    int count = 0;
    Object* curr = vm->free_list;
    while (curr) {
        count++;
        curr = curr->right;
    }
    return count;
}

int gc(VM* vm) {
    //Snapshot: How many slots were ALREADY free?
    int initial_free_count = count_free_list(vm);

    // mark
    // iterate through the VM stack to find roots
    for (int i = 0; i < vm->st_ptr; i++) {
        Object* obj = (Object*)vm->stack[i]; // cast int to ptr
        
        if (obj >= vm->heap && obj < vm->heap + HEAP_SIZE) {
            // valid ptr in our heap range
            mark_object(obj);
        }
    }

    // sweep
    vm->free_list = NULL;
    Object* curr_free = NULL; // tracks end of new free_list
    int final_free_count = 0; // total slots available after GC 


    for (int i = 0; i < HEAP_SIZE; i++) {
        if (vm->heap[i].marked) {
            // obj is alive, unmark it for next cycle
            vm->heap[i].marked = false;
        } else {
            // obj is dead, add to free list
            final_free_count++;
            if (vm->free_list == NULL) {
                vm->free_list = &vm->heap[i];
                curr_free = vm->free_list;
            } else {
                curr_free->right = &vm->heap[i];
                curr_free = curr_free->right;
            }
        }
    }

    // terminate the free list
    if (curr_free) {
        curr_free->right = NULL;
    }
    
    // actually_reclaimed = (Space After GC) - (Space Before GC)
    int actually_reclaimed = final_free_count - initial_free_count;
    
    // Safety check (should not happen, but good for debug)
    if (actually_reclaimed < 0) actually_reclaimed = 0;
    vm->total_freed += actually_reclaimed;
    return actually_reclaimed;
}



int get_reachable_count(VM* vm) {
    // 1. Reset: Ensure no leftover marks exist (start fresh)
    for (int i = 0; i < HEAP_SIZE; i++) {
        vm->heap[i].marked = false;
    }

    // 2. Mark: Scan the stack to find all reachable objects
    // (This logic is identical to the Mark phase in your GC)
    for (int i = 0; i < vm->st_ptr; i++) {
        Object* obj = (Object*)vm->stack[i];
        
        // Safety check: Is this stack value actually a pointer to our heap?
        if (obj >= vm->heap && obj < vm->heap + HEAP_SIZE) {
            mark_object(obj);
        }
    }
    // 2b. Mark: Scan Global Memory (The missing piece!)
    for (int i = 0; i < MEM_SIZE; i++) {
        Object* obj = (Object*)vm->memory[i]; // Cast int to ptr

        // Safety check: Is this pointing to our heap?
        if (obj >= vm->heap && obj < vm->heap + HEAP_SIZE) {
            mark_object(obj);
        }
    }

    // 3. Count: Tally up everything we just marked
    int count = 0;
    for (int i = 0; i < HEAP_SIZE; i++) {
        if (vm->heap[i].marked) {
            count++;
        }
    }

    // 4. Cleanup: Unmark everything so we don't break the next real GC!
    for (int i = 0; i < HEAP_SIZE; i++) {
        vm->heap[i].marked = false;
    }

    return count;
}

