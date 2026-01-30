CXX = g++
CC = gcc
CFLAGS = -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -g
CXXFLAGS = -I src -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -g -std=c++11

# Sub-module sources
SHELL_SRC = src/shell/apsh_background.c src/shell/apsh_export.c src/shell/apsh_cd.c \
            src/shell/apsh_handle_sig.c src/shell/apsh_exit.c src/shell/apsh_execute_pipeline.c \
            src/shell/apsh_add_prompt.c src/shell/lru_history.c src/shell/shell_interface.cpp

BVM_SRC = src/bvm/vm/bvm.cpp src/bvm/gc/gc.cpp src/bvm/assembler/assembler.cpp

PARSER_SRC = src/parser/ast.c src/parser/lex.yy.c src/parser/parser.tab.c

MAIN_SRC = src/main_integrated.cpp src/compiler.cpp

# Output binary
TARGET = integrated_system

all: parser $(TARGET)

# 1. Generate Parser C files from Flex/Bison
parser:
	bison -d src/parser/parser.y -o src/parser/parser.tab.c
	flex -o src/parser/lex.yy.c src/parser/lexer.l

# 2. Compile and Link everything
$(TARGET): parser
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_SRC) $(BVM_SRC) $(SHELL_SRC) $(PARSER_SRC) -lfl

clean:
	rm -f $(TARGET) src/parser/parser.tab.c src/parser/parser.tab.h src/parser/lex.yy.c