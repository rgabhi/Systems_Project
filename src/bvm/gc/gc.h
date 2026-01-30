#ifndef GC_H
#define GC_H

#include <cstddef> // for NULL

// forward decl of VM to avoid circular dependency
class VM;

struct Object {
    bool marked;       // Mark bit
    Object* left;      // Reference 1
    Object* right;     // Reference 2
};

// --- GC conventn / wrapper funcs ---

// alloc a new object pair in the VM's heap
Object* new_pair(VM* vm, Object* l, Object* r);
Object* new_function(VM* vm);
Object* new_closure(VM* vm, Object* fn, Object* env);

// push obj reference on VM stack
void push(VM* vm, Object* obj);

// pops object reference from VM stack
Object* pop(VM* vm);

// Triggers Mark-Sweep Garbage Collector
int gc(VM* vm);

// Helper macro for convention
#define VAL_OBJ(x) (x)

#endif