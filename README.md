# 🐚 AP Shell & Systems Virtual Machine

> A comprehensive systems programming project integrating a custom Linux shell with a fully functional compiler and virtual machine for a custom C-like scripting language.


---

## 📋 Table of Contents

- [Overview](#-overview)
- [Key Features](#-key-features)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage Guide](#-usage-guide)
  - [Shell Commands](#shell-commands)
  - [Script Execution](#script-execution)
  - [Debugging](#debugging)
- [AP Language Reference](#-ap-language-reference)
- [Architecture](#-architecture)
- [Examples](#-examples)
- [Technical Details](#-technical-details)

---

## 🚀 Overview

This project demonstrates the complete lifecycle of code execution—from source code to runtime execution. It bridges high-level programming concepts with low-level systems programming through three integrated components:

### Core Components

| Component | Description | Technologies |
|-----------|-------------|--------------|
| **AP Shell** | Custom command-line interface with process management, pipelines, and signal handling | C++, POSIX APIs |
| **Compiler Pipeline** | Multi-stage compilation from `.lang` files to bytecode | Flex, Bison, C++ |
| **Bytecode VM (BVM)** | Stack-based virtual machine with memory management and garbage collection | C++, Custom ISA |

### Compilation Flow

```
.lang Source → Lexer → Parser → AST → IR → Bytecode → VM Execution
```

---

## ✨ Key Features

### 🖥️ Custom Shell (apshell)

- **Command Execution** – Run standard Linux commands (`ls`, `grep`, `cat`, etc.)
- **Pipeline Support** – Chain commands with pipes: `command1 | command2 | command3`
- **Background Jobs** – Execute processes asynchronously using `&`
- **Built-in Commands** – Custom implementations of `cd`, `exit`, `export`, `history`
- **Signal Handling** – Graceful handling of `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z)
- **Job Control** – Manage foreground and background processes

### 📝 AP Language

A high-level scripting language with C-like syntax supporting:

| Feature | Syntax | Description |
|---------|--------|-------------|
| **Variables** | `var x = 10;` | Integer variable declaration |
| **Control Flow** | `if/else`, `while`, `for` | Standard control structures |
| **Heap Allocation** | `heap(value)` | Dynamic memory allocation |
| **Pointer Dereferencing** | `@ptr` | Access heap-allocated values |
| **Arithmetic** | `+`, `-`, `*`, `/`, `%` | Standard operators |
| **Comparisons** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Relational operators |

### ⚙️ Virtual Machine (BVM)

**Architecture:**
- **Stack-Based Design** – 64-bit operand stack for computation
- **Three-Tier Memory Model:**
  - **Stack** – Operand manipulation during execution
  - **Memory** – Variable storage (64-bit addressable)
  - **Heap** – Dynamic object allocation with pointer arithmetic

**Features:**
- **Garbage Collection** – Mark-and-sweep GC prevents memory leaks
- **Interactive Debugger** – Step through execution, inspect memory state
- **Rich Instruction Set** – 30+ opcodes for computation and control flow
- **Type Safety** – Runtime type checking for pointer operations

---
## 📂 Project Structure

```
systems_project/
│
├── src/
│   ├── shell/                      # Shell Implementation
│   │   ├── apshell.cpp             # Main shell loop
│   │   ├── process.cpp             # Process management
│   │   ├── signals.cpp             # Signal handlers
│   │   ├── apsh_add_prompt.c       # Prompt handling
│   │   ├── apsh_background.c       # Background process management
│   │   ├── apsh_cd.c               # Change directory built-in
│   │   ├── apsh_execute_pipeline.c # Pipeline execution
│   │   ├── apsh_exit.c             # Exit command
│   │   ├── apsh_export.c           # Environment variable export
│   │   ├── apsh_handle_sig.c       # Signal handling
│   │   ├── apsh_lifecycle.c        # Process lifecycle management
│   │   └── apsh_module.h           # Shell module header
│   │
│   ├── parser/                     # Language Frontend
│   │   ├── lexer.l                 # Flex lexical analyzer
│   │   ├── parser.y                # Bison grammar rules
│   │   ├── ast.c                   # Abstract Syntax Tree
│   │   ├── ast.h                   # AST header
│   │   ├── parser_wrapper.c        # Parser wrapper functions
│   │   └── level1                  # Parser level 1
│   │
│   ├── ir/                         # Intermediate Representation
│   │   ├── ir.c                    # IR implementation
│   │   ├── ir.h                    # IR header
│   │   └── vm_bridge.cpp           # Bridge between IR and VM
│   │
│   ├── bvm/                        # Virtual Machine Backend
│       ├── assembler/              # Bytecode Generation
│       │   ├── assembler.cpp       # IR to bytecode translation
│       │   └── assembler.h         # Assembler header
│       │
│       ├── vm/                     # Core VM implementation
│       │   ├── executor.cpp        # Instruction execution loop
│       │   ├── stack.cpp           # Operand stack
│       │   └── memory.cpp          # Memory management
│       │
│       ├── gc/                     # Garbage Collector
│       │   ├── gc.cpp              # Mark-and-sweep implementation
│       │   └── allocator.cpp       # Heap allocator
│       │
│       ├── bvm.cpp                 # BVM main implementation
│       ├── bvm.h                   # BVM header
│       └── commons.h               # Common definitions
│   
│   
│   
│   
│   
│
├── tests/                          # Test Programs
│   ├── demo.lang                   # Demo program
│   ├── demo2.lang                  # Additional demo
│   ├── heap_test.lang              # Heap allocation tests
│   ├── stress_test.lang            # Performance stress tests
│   ├── test_for.lang               # For loop tests
│   ├── test_heap.lang              # Heap operation tests
│   ├── test_unary.lang             # Unary operator tests
│   ├── test_while.lang             # While loop tests
│   └── test1.lang                  # General test cases
│
├── Makefile                        # Build configuration
└── README.md                       # This file
```

---

## 🚀 Getting Started

### Prerequisites

Ensure you have the following tools installed:

```bash
# C++ Compiler (g++ 9.0 or later)
g++ --version

# Flex (lexical analyzer generator)
flex --version

# Bison (parser generator)
bison --version

# Make (build automation)
make --version
```

**Installation Commands** (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install build-essential flex bison
```

### Installation

1. **Clone the Repository**
   ```bash
   git clone https://github.com/yourusername/systems_project.git
   cd systems_project
   ```

2. **Build the Project**
   ```bash
   make apshell
   ```

3. **Verify Installation**
   ```bash
   ./apshell
   ```

4. **Clean Build** (if needed)
   ```bash
   make clean
   make apshell
   ```

---

## 💻 Usage Guide

### Launching the Shell

```bash
./apshell
```

You should see the prompt:
```
AP_SHELL >>
```

### Shell Commands

#### Standard Commands
All standard Linux commands work as expected:

```bash
AP_SHELL >> ls -la
AP_SHELL >> pwd
AP_SHELL >> echo "Hello, World!"
AP_SHELL >> cat file.txt | grep "pattern"
```

#### Built-in Commands

| Command | Description | Example |
|---------|-------------|---------|
| `cd <directory>` | Change working directory | `cd /home/user` |
| `exit` | Exit the shell | `exit` |
| `export VAR=value` | Set environment variable | `export PATH=/usr/bin` |
| `history` | Show command history | `history` |

### Script Execution

#### 1. Submit a Script

Compile a `.lang` file and register it with a Process ID (PID):

```bash
AP_SHELL >> submit tests/heap_test.lang
```

**Output:**
```
Integrating Lab 2: Parsing tests/demo.lang...
Program successfully parsed and instrumented. (PID: 1)

```

#### 2. Run the Script

Execute the compiled bytecode:

```bash
AP_SHELL >> run 1
```

**Output:**
```
[Program output here]
Executing PID 1 (tests/demo.lang)...

Execution completed successfully
```


### Debugging

#### Enter Debug Mode

```bash
AP_SHELL >> debug 1
```

You'll enter an interactive debugger:
```
Entering debug mode for PID 1
Debugger ready. Type 'h' for help.
(dbg) >>
```

#### Debugger Commands

| Command | Description | Example |
|---------|-------------|---------|
| `s` or `step` | Execute next instruction | `s` |
| `c` or `continue` | Run until breakpoint or end | `c` |
| `m` or `memory` | Display memory statistics | `m` |
| `i <addr>` | Inspect object at heap address | `i 0x1A2B` |
| `b <line>` | Set breakpoint at line | `b 15` |
| `q` or `quit` | Exit debugger | `q` |

#### Example Debug Session

```
(dbg) >> s
Executed: PUSH 1234
Stack: [1234]
PC: 1

(dbg) >> m
=== Memory Statistics ===
Stack Size: 1
Variables: 0
Heap Objects: 0
Heap Usage: 0 bytes

(dbg) >> c
Execution completed
Final stack: [90]
```

---

## 📝 AP Language Reference

### Syntax Overview

#### Variables
```javascript
var x = 10;           // Integer variable
var y = x + 5;        // Arithmetic expression
var ptr = heap(100);  // Heap allocation returns pointer
```

#### Control Flow
```javascript
// If-Else
if (x > 10) {
    y = 1;
} else {
    y = 0;
}

// While Loop
while (x < 100) {
    x = x + 1;
}

// For Loop
for (var i = 0; i < 10; i = i + 1) {
    // Loop body
}
```

#### Heap Operations
```javascript
// Allocate integer on heap
var h = heap(42);

// Dereference heap pointer
var value = @h;      // or *h

// Check heap value
if (@h == 42) {
    // Condition is true
}

```

---

## 🏗️ Architecture

### Compilation Pipeline

```
┌─────────────┐
│ .lang File  │
└──────┬──────┘
       │
       ▼
┌─────────────┐     Tokenization
│    Lexer    │ ◄── (Flex)
└──────┬──────┘
       │
       ▼
┌─────────────┐     Syntax Analysis
│   Parser    │ ◄── (Bison)
└──────┬──────┘
       │
       ▼
┌─────────────┐     Abstract Syntax Tree
│     AST     │
└──────┬──────┘
       │
       ▼
┌─────────────┐     Optimization
│  IR Gen     │ ◄── (SSA Form)
└──────┬──────┘
       │
       ▼
┌─────────────┐     Code Generation
│  Assembler  │
└──────┬──────┘
       │
       ▼
┌─────────────┐
│  Bytecode   │
└──────┬──────┘
       │
       ▼
┌─────────────┐     Execution
│     BVM     │ ◄── (Stack-based)
└─────────────┘
```

### Virtual Machine Design

**Instruction Execution Cycle:**

```
1. Fetch instruction at PC (Program Counter)
2. Decode opcode and operands
3. Execute operation (modify stack/memory/heap)
4. Update PC
5. Check for GC trigger
6. Repeat
```

**Memory Layout:**

```
┌──────────────────────┐
│   Operand Stack      │ ← Push/Pop operations
├──────────────────────┤
│   Local Variables    │ ← Named storage
├──────────────────────┤
│   Heap               │ ← Dynamic allocation
│   (GC Managed)       │
└──────────────────────┘
```

### Instruction Set Architecture (ISA)

#### Core Instructions

| Opcode  | Operands | Description |
|---------|----------|-------------|
| `PUSH`  | `<value>` | Push immediate value onto stack |
| `POP`   | - | Remove top of stack |
| `STORE` | `<var_id>` | Store top of stack to variable |
| `LOAD`  | `<var_id>` | Load variable onto stack |
| `ADD`   | - | Pop two values, push sum |
| `SUB`   | - | Pop two values, push difference |
| `MUL`   | - | Pop two values, push product |
| `DIV`   | - | Pop two values, push quotient |
| `MOD`   | - | Pop two values, push remainder |

#### Control Flow

| Opcode | Operands  | Description |
|--------|-----------|-------------|
| `JMP`  | `<label>` | Unconditional jump |
| `JZ`   | `<label>` | Jump if top of stack is zero |
| `JNZ`  | `<label>` | Jump if top of stack is non-zero |
| `CMP`  | - | Compare top two stack values |

#### Memory Operations

| Opcode  | Operands | Description |
|---------|----------|-------------|
| `ALLOC` | - | Allocate heap object with value from stack |
| `DEREF` | - | Dereference pointer on stack |
| `FREE`  | `<addr>` | Mark object for garbage collection |

---

## 📚 Examples

### Example 1: Simple Arithmetic

**File:** `tests/demo.lang`

```javascript
// Test Variables & Logic
var x = 10;
var y = 20;
var result = 0;

if (x < y) {
    result = y - x; // Should be 10
}

// Test Loop & Allocation (for GC/Memstat)
var i = 0;
while (i < 5) {
    // Creating "objects" (logic depends on your parser/VM support for objects)
    // If your language only supports ints, this loop just tests flow control.
    result = result + 1; 
    i = i + 1;
}
```

**Execution:**
```bash
AP_SHELL >> submit tests/arithmetic.lang
Program successfully parsed... (PID: 1)

AP_SHELL >> run 1
Executing PID 1 (tests/demo.lang)...
Lowering PID 1 to IR...
Dispatching to VM...
--- BVM managed exec starting ---
Stack: [Empty]
Memory[0] 10
--- BVM exec complete ---
```

### Example 2: Heap Allocation

**File:** `tests/test_heap.lang`

```javascript
// Allocate integer 1234 on the heap
var h = heap(1234);
var x = 0;

// Dereference heap pointer and check value
if (@h == 1234) {
    x = 90;      // Condition is true
} else {
    x = 1000;
}

// x = 90
```

**Debug Session:**
```bash
AP_SHELL >> debug 1
=== BVM Debugger: PID 1 ===
Commands: [s]tep, [c]ontinue, [b]reak <addr>, [i]nspect <addr>, [m]emstat, [q]uit
dbg@L002:PC_000> s
  Stack Top: 0
dbg@L002:PC_005> s
dbg@L003:PC_010> s
  Stack Top: 1234
dbg@L003:PC_015> c
addr---->140726106027976
=== Debugger Session Terminated ===

```

### Example 3: While Loop

**File:** `tests/test_while.lang`

```javascript

var x=2;

while(x<=8){
    x=x*2;
}

```

**Execution:**
```bash
AP_SHELL >> submit tests/arithmetic.lang
Executing PID 1 (tests/test_while.lang)...
Lowering PID 1 to IR...
Dispatching to VM...
--- BVM managed exec starting ---
Stack: [Empty]
Memory[0] 16
--- BVM exec complete ---

```


```



---

## 🔧 Technical Details

### Memory Management

**Pointer Representation:**
- Uses 64-bit `long long` for stack and memory
- Ensures valid pointer storage on 64-bit systems
- Heap addresses are virtual indices converted to raw pointers during dereferencing

**Garbage Collection:**
- **Algorithm:** Mark-and-Sweep
- **Trigger:** Automatic when heap usage exceeds threshold
- **Roots:** Stack, local variables, global variables
- **Phase 1 (Mark):** Traverse from roots, mark reachable objects
- **Phase 2 (Sweep):** Free unmarked objects

### Type System

| Type      | Size   | Description           |
|-----------|--------|-----------------------|
| `Integer` | 64-bit | Signed integer values |
| `Pointer` | 64-bit | Heap object reference |

### Performance Considerations

- **Stack Size:** 1024 elements (configurable)
- **Heap Growth:** Dynamic, grows as needed
- **Instruction Dispatch:** Direct threaded code for performance

### Safety Features

- **Bounds Checking:** Array/stack overflow detection
- **Null Pointer Protection:** Prevents dereferencing null
- **Type Safety:** Runtime type checking for operations
- **Memory Safety:** Automatic garbage collection prevents leaks



