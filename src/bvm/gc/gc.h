#ifndef GC_H
#define GC_H

#include <cstddef> // for NULL

// forward decl of VM to avoid circular dependency
class VM;

// 1. Add an Enum to distinguish types
typedef enum { OBJ_PAIR, OBJ_INT } ObjectType;

struct Object {
    bool marked;       // Mark bit
    ObjectType type;   //  Track what this object is
    int value;         //  Store the integer data here
    Object* left;      // Reference 1 // Only used if type == OBJ_PAIR
    Object* right;     // Reference 2
};

// --- GC conventn / wrapper funcs ---

// alloc a new object pair in the VM's heap
Object* new_pair(VM* vm, Object* l, Object* r);
Object* new_function(VM* vm);
Object* new_closure(VM* vm, Object* fn, Object* env);
// 2. Add a prototype for your new allocator
Object* new_int(VM* vm, int value);

int count_free_list(VM* vm);

// push obj reference on VM stack
void push(VM* vm, Object* obj);

// pops object reference from VM stack
Object* pop(VM* vm);

// Triggers Mark-Sweep Garbage Collector
int gc(VM* vm);
int get_reachable_count(VM* vm);

// Helper macro for convention
#define VAL_OBJ(x) (x)

#endif