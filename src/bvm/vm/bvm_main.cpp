#include "bvm.h"


void load_bytecode(unsigned char *buffer, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Bytecode file");
        exit(1);
    }
    fread(buffer, 1, CODE_SIZE, f);
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s program.bin\n", argv[0]);
        return 1;
    }

    unsigned char code_buffer[CODE_SIZE];
    memset(code_buffer, 0, CODE_SIZE);

    load_bytecode(code_buffer, argv[1]);

    VM vm(code_buffer);

  
    vm.run();

    if(vm.st_ptr > 0){   
        printf("Final stack top: %lld\n", vm.stack[vm.st_ptr - 1]);
    } else {
        printf("Stack empty.\n");
    }

    return 0;
}