#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <cstdint>
#include <iostream>
#include <cstring>
#include <map>
#include <string>

#define CODE_SIZE  1024 
#define LINE_SIZE 128

// Helper functions for string processing
void trim(char *str);
void clean_line(char *line);

// pass 1: identify labels & calculate addresses
int parse_labels(char *line, int pc, std::map<std::string, int> &labels);

// pass 2: generate code using the label map
int assemble_line(char *line, uint8_t *code, int pc, const std::map<std::string, int> &labels);

// helper for emitting instructions
int emit(uint8_t *code, int pc, uint8_t byte);
int emit_int(uint8_t *code, int pc, int value);

#endif