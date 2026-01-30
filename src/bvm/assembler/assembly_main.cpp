#include "assembler.h"
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input.asm output.bin\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "wb");
    if (!in || !out) { perror("File error"); return 1; }

    uint8_t code[CODE_SIZE];
    int pc = 0;
    char line[LINE_SIZE];
    std::map<std::string, int> labels;


    // --- PASS 1: Map Labels ---
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == ';') continue;
        pc = parse_labels(line, pc, labels);
    }

    // Reset file pointer and PC for Pass 2
    fseek(in, 0, SEEK_SET);
    pc = 0;

    // --- PASS 2: Generate Code ---
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == ';') continue;
        pc = assemble_line(line, code, pc, labels);
    }

    fwrite(code, 1, pc, out);
    fclose(in);
    fclose(out);

    printf("Assembled %d bytes into %s\n", pc, argv[2]);
    return 0;
}