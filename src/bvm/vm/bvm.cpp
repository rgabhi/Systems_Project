#include "bvm.h"
#include "../commons.h"


VM::VM(unsigned char* bytecode) {
            this->program = bytecode;
            this->inst_ptr = bytecode;
            this->st_ptr = 0;
            this->max_sp = 0;
            this->total_allocs = 0;
            this->total_freed = 0;
            this->rst_ptr = 0;
            this->instruction_cnt = 0;
            this->running = true;
            
            // init memory to zero
            memset(this->memory, 0, sizeof(this->memory));
            
            // init heap (Link all objects into the free list initially)
            for(int i = 0; i < HEAP_SIZE - 1; i++){
                this->heap[i].right = &this->heap[i + 1];
                this->heap[i].marked = false;  
            }
            this->heap[HEAP_SIZE -1].right = NULL;
            this->heap[HEAP_SIZE -1].marked = false; 
    
            this->free_list = &this->heap[0];
}


// helper to check st size
bool VM::check_stack(int count) {
    if (this->st_ptr < count) {
        printf("Error: Stack Underflow\n");
        this->running = false;
        return false;
    }
    return true;
}

void VM::run(){
    this->running = true;
    while(running){
        
        step();

    }
    
}

    // Getter for the benchmark tool to access the private counter
long long VM::getInstructionCnt(){ return instruction_cnt; }


void VM::step() {
    if (!running) return;
    this->instruction_cnt++;// increment per inst cycle
    // 1. FETCH : get curr instruction
    unsigned char opcode = *this->inst_ptr;
    
    // 2. DECODE: decide what to do
    switch(opcode){
        case HALT: // HALT
        {
            running = false;
            break;
        }
        case PUSH: //PUSH
        {    // get addr of the byte after opcode
            // (int *):  cast it: treat addr as ptr to int
            // * : dereference: read int val
            if(st_ptr >= STACK_SIZE){
                printf("Stack Overflow\n");
                running = false;
                break;
            }
            int val = *(int *)(this->inst_ptr + 1);
            // stack logic
            // add val to stack
            // update stack ptr
            this->stack[this->st_ptr] = val;
            this->st_ptr++; 

            // move to next inst by jumping 4 byte int
            this->inst_ptr += 4;
            break;
        }
        case POP: { // POP
            if(check_stack(1)){
                this->st_ptr--;
            }
            break;
        }
        case ADD: // ADD
        {
            if(!check_stack(2)){
                break;
            }
            this->st_ptr--;
            int b = this->stack[this->st_ptr];
            this->st_ptr--;
            int a = this->stack[this->st_ptr];
            b = a + b;
            this->stack[this->st_ptr] = b;
            this->st_ptr++;
            break;
        }
        case SUB: // SUBTRACT
        {
            if(!check_stack(2)){
                break;
            }
            this->st_ptr--;
            int b = this->stack[this->st_ptr];
            this->st_ptr--;
            int a = this->stack[this->st_ptr];
            int res = a - b;
            this->stack[this->st_ptr] = res;
            this->st_ptr++;
            break;
        }
        case MUL: // MUL
        {
            if(!check_stack(2)){
                break;
            }
            this->st_ptr--;
            int b = this->stack[this->st_ptr];
            this->st_ptr--;
            int a = this->stack[this->st_ptr];
            int res = a*b;
            this->stack[this->st_ptr] = res;
            this->st_ptr++;
            break;
        }
        case DIV: // DIV
        {
            if(!check_stack(2)){
                break;
            }
            this->st_ptr--;
            int b = this->stack[this->st_ptr];
            if (b==0){
                printf("Error: Div by Zero\n");
                running = false;
                break;
            }
            this->st_ptr--;
            int a = this->stack[this->st_ptr];
            int res = (int)a/b;
            this->stack[this->st_ptr] = res;
            this->st_ptr++;
            break;
        }

        case STORE: //  STORE IDX
        {
            // pop val from stack
            this->st_ptr--;
           long long int val =this->stack[this->st_ptr];
            // get idx 
            int idx = *(int *) (this->inst_ptr + 1);
            this->memory[idx] = val;
            // move inst_ptr
            this->inst_ptr += 4;
            break;
        }
        case LOAD: // LOAD IDX
        {
            if (st_ptr >= STACK_SIZE) {
                printf("Stack Overflow\n");
                running = false;
                break;
            }
            // get idx
            int idx = *(int *)(this->inst_ptr + 1);
            
            // read val from memory
            long long int val = this->memory[idx];
            // push to stack
            this->stack[this->st_ptr] = val;
            this->st_ptr++;

            // move inst_ptr
            this->inst_ptr += 4;
            break;
        }
        
        case JMP: // JUMP addr
        {
            // get idx
            int target = *(int *)(this->inst_ptr + 1);

            //update ptr
            this->inst_ptr = this->program + target;
            //jump to next instr
            return;
            break;
        }
        case JZ: // JZ addr
        {
            if(!check_stack(1)) break;
            // pop val
            this->st_ptr--;
            int val = this->stack[this->st_ptr];

            // target address
            int target = *(int *)(this->inst_ptr + 1);

            if(val == 0){
                this->inst_ptr = this->program + target;
                return;
            }
            else{
                //
                this->inst_ptr += 4;
                break;
            }
            break;
        }
        case JNZ: // JNZ addr
        {
            if (!check_stack(1)) break;
            // pop val
            this->st_ptr--;
            int val = this->stack[this->st_ptr];

            // target address
            int target = *(int *)(this->inst_ptr + 1);

            if(val != 0){
                this->inst_ptr = this->program + target;
                return;
            }
            else{
                //
                this->inst_ptr += 4;
                break;
            }
            break;
        }
        case DUP:
        {
            if (st_ptr >= STACK_SIZE) {
                printf("Stack Overflow\n");
                running = false;
                break;
            }
            int top = this->stack[this->st_ptr - 1];
            this->stack[this->st_ptr] = top;
            this->st_ptr++;
            break;

        }
        case CALL: // CALL addr
        { 
            // get target jump addr
            int target = *(int *)(this->inst_ptr + 1);

            // calc return addr (curr location + 5 bytes)
            // store this as an OFFSET from start of prog
            int return_addr = (int)(this->inst_ptr - this->program) + 5;

            // push to RETURN STACK
            this->ret_stack[this->rst_ptr] = return_addr;
            this->rst_ptr++;

            // jump
            this->inst_ptr = this->program + target;
            return;

          
        }

        case RET: // RET
        { 
        // check if we have anywhere to return to
        if (this->rst_ptr == 0) {
            printf("Error: Stack underflow on RET\n");
            running = false;
            break;
        }
    }
        case CMP: { // Keep this for Less Than (<)
            if(!check_stack(2)) break;
            this->st_ptr--;
            int b = this->stack[this->st_ptr];
            this->st_ptr--;
            int a = this->stack[this->st_ptr];
            int res = (a < b) ? 1 : 0;
            this->stack[this->st_ptr] = res;
            this->st_ptr++;
            break;
        }
        case ALLOC:
        {
            // 1. Pop the value (the integer size/value) from the stack
            int val = (int)this->stack[--this->st_ptr];

            // 2. Allocate the object on the heap using our new GC function
            Object* obj = new_int(this, val);

            // 3. Push the new Object's memory address back onto the stack
            this->stack[this->st_ptr++] = (long long)obj;
            break;

        }

        // ADD THIS NEW CASE
       case EQ: { 
            if(!check_stack(2)) break;
            this->st_ptr--; int b = this->stack[this->st_ptr];
            this->st_ptr--; int a = this->stack[this->st_ptr];
            this->stack[this->st_ptr++] = (a == b) ? 1 : 0;
            break;
        }

        case LE: { 
            if(!check_stack(2)) break;
            this->st_ptr--; int b = this->stack[this->st_ptr];
            this->st_ptr--; int a = this->stack[this->st_ptr];
            this->stack[this->st_ptr++] = (a <= b) ? 1 : 0;
            break;
        }

        // NEW: GE is Greater Equal (>=)
        case GE: { 
            if(!check_stack(2)) break;
            this->st_ptr--; int b = this->stack[this->st_ptr];
            this->st_ptr--; int a = this->stack[this->st_ptr];
            this->stack[this->st_ptr++] = (a >= b) ? 1 : 0;
            break;
        }

        case NEQ: { 
            if(!check_stack(2)) break;
            this->st_ptr--; int b = this->stack[this->st_ptr];
            this->st_ptr--; int a = this->stack[this->st_ptr];
            this->stack[this->st_ptr++] = (a != b) ? 1 : 0;
            break;
        }

        case NEG: {
            if(!check_stack(1)) break;
            this->st_ptr--;
            int val = this->stack[this->st_ptr];
            this->stack[this->st_ptr++] = -val;
            break;
        }

        case DEREF: // 0x1E
        {
            if(!check_stack(1)) break;
            
            // 1. Pop the Raw Address (long long)
            this->st_ptr--;
            long long addr = this->stack[this->st_ptr];

            printf("addr---->%lld",addr);

            // 2. Cast the integer back to an Object Pointer
            Object* obj_ptr = (Object*)addr;

            // 3. Safety Check: Ensure this pointer is actually inside our heap array
            //    (Pointer Arithmetic: address comparison)
            if (obj_ptr < this->heap || obj_ptr >= (this->heap + HEAP_SIZE)) {
                printf("Error: Segmentation Fault (Address %lld is out of heap bounds)\n", addr);
                running = false;
                break;
            }

            // 4. Access the value
            int val = obj_ptr->value;
            
            // 5. Push the value back to the stack
            this->stack[this->st_ptr++] = val;
            break;
        }

        default:
            printf("Unknown Opcode %x\n", opcode);
            running = false;
            break;

    }
    if(this->st_ptr > this->max_sp){
        this->max_sp = this->st_ptr;
    }
    // move to next instruction (if not stopping)
    if(running){
        this->inst_ptr++;
    }
}

void VM::inspect_heap_addr(long long addr) {
    // 1. Cast the integer address back to an Object pointer
    Object* obj = (Object*)addr;

    // 2. Safety Check: Ensure the address is actually inside our heap array
    // (This prevents segfaults if we type a wrong number)
    if (obj < this->heap || obj >= (this->heap + HEAP_SIZE)) {
        printf("Error: Address %lld is not within the managed heap.\n", addr);
        return;
    }

    // 3. Print the details
    printf("Object at address %lld:\n", addr);
    if (obj->type == OBJ_INT) {
        printf("  [Type]: INT\n");
        printf("  [Value]: %d\n", obj->value);
    } else if (obj->type == OBJ_PAIR) {
        printf("  [Type]: PAIR\n");
        printf("  [Left]: %p, [Right]: %p\n", (void*)obj->left, (void*)obj->right);
    } else {
        printf("  [Type]: UNKNOWN (Tag: %d)\n", obj->type);
    }
}
