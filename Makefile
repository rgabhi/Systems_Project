CXX = g++
CC = gcc
# Added -fpermissive to CFLAGS to handle pointer conversions if compiled with g++ 
# though the shell target uses gcc specifically.
CFLAGS = -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -I src/ir -g 
CXXFLAGS = -I src -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -I src/ir -g -std=c++11

# Sub-module sources for integrated system
SHELL_SRC = src/shell/apsh_background.c src/shell/apsh_export.c src/shell/apsh_cd.c \
            src/shell/apsh_handle_sig.c src/shell/apsh_exit.c src/shell/apsh_execute_pipeline.c \
            src/shell/apsh_add_prompt.c src/shell/lru_history.c src/shell/shell_interface.cpp

BVM_SRC = src/bvm/vm/bvm.cpp src/bvm/gc/gc.cpp src/bvm/assembler/assembler.cpp

PARSER_SRC = src/parser/ast.c src/parser/lex.yy.c src/parser/parser.tab.c

MAIN_SRC = src/main_integrated.cpp src/compiler.cpp

# Standalone Shell Source Files (Updated with correct relative paths)
STANDALONE_SHELL_SRCS = src/shell/main.c\
                        src/shell/apsh_cd.c \
                        src/shell/apsh_execute_pipeline.c \
                        src/shell/apsh_exit.c \
                        src/shell/apsh_handle_sig.c \
                        src/shell/apsh_background.c \
                        src/shell/apsh_export.c \
                        src/shell/lru_history.c \
                        src/shell/apsh_add_prompt.c \
						src/shell/apsh_lifecycle.c \
						src/parser/parser.tab.c \
                        src/parser/lex.yy.c \
                        src/parser/ast.c \
                        src/parser/parser_wrapper.c \
						src/ir/ir.c \
						src/ir/vm_bridge.cpp \
						src/bvm/vm/bvm.cpp \
						src/bvm/gc/gc.cpp \
						src/bvm/assembler/assembler.cpp \
						
						


# 2. Compile and Link Integrated System
$(TARGET): parser
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(MAIN_SRC) $(BVM_SRC) $(SHELL_SRC) $(PARSER_SRC) -lfl









# Makefile

# Final binary name
TARGETM = apshell

# Core source files for the integrated shell
# REMOVED: assembly_main.cpp to avoid multiple 'main' definitions
INTEGRATED_SRCS = src/shell/main.c \
                  src/shell/apsh_cd.c \
                  src/shell/apsh_execute_pipeline.c \
                  src/shell/apsh_exit.c \
                  src/shell/apsh_handle_sig.c \
                  src/shell/apsh_background.c \
                  src/shell/apsh_export.c \
                  src/shell/lru_history.c \
                  src/shell/apsh_add_prompt.c \
                  src/shell/apsh_lifecycle.c \
                  src/parser/parser.tab.c \
                  src/parser/lex.yy.c \
                  src/parser/ast.c \
                  src/parser/parser_wrapper.c \
                  src/ir/ir.c \
                  src/ir/vm_bridge.cpp \
                  src/bvm/vm/bvm.cpp \
                  src/bvm/gc/gc.cpp \
                  src/bvm/assembler/assembler.cpp

# Compiler flags
CFLAGS_INT = -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -I src/ir -g

shell: parser
	# Link with g++ to include C++ standard libraries automatically
	g++ $(CFLAGS_INT) $(INTEGRATED_SRCS) -o $(TARGETM) -lfl

parser:
	bison -d src/parser/parser.y -o src/parser/parser.tab.c
	flex -o src/parser/lex.yy.c src/parser/lexer.l







clean:
	rm -f $(TARGET) $(TARGETM) src/parser/parser.tab.c src/parser/parser.tab.h src/parser/lex.yy.c