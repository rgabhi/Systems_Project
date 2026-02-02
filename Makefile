# Target Name
TARGETM = apshell

# Source files
# Note: We include the C++ files (vm_bridge, bvm, gc) here so the linker finds them
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

# Flags
CFLAGS_INT = -I src/shell -I src/parser -I src/bvm/vm -I src/bvm/gc -I src/bvm/assembler -I src/ir -g

# --- Parser Generation (The Missing Ingredient!) ---
parser:
	bison -d -o src/parser/parser.tab.c src/parser/parser.y
	flex -o src/parser/lex.yy.c src/parser/lexer.l

# Build Rule
# We use g++ to link because we have .cpp files mixed with .c files
$(TARGETM): parser
	g++ $(CFLAGS_INT) $(INTEGRATED_SRCS) -o $(TARGETM) 
#-lfl removed: Since we are providing our own main function in the shell, we don't need the default one from the library either

clean: ; rm -f $(TARGETM) src/parser/parser.tab.c src/parser/parser.tab.h src/parser/lex.yy.c