# Recursive Factorial Program 

## Assignment Overview

This project demonstrates a recursive factorial calculation implemented in both C and assembly language, running on a custom 16-bit RISC CPU emulator. The assignment showcases:

1. **C recursive function** with a main driver program
2. **Executable memory layout** in the virtual CPU
3. **Function call mechanism** (CALL/RET instructions)
4. **Recursion execution** with stack frame management

```

## Quick Start

### 1. Build the Tools

bash
make clean
make all
```

This creates:
- `build/assembler` - Assembles .asm files to binary
- `build/emulator` - Executes binary programs on virtual CPU

### 2. Run the C Version

```bash
gcc -o build/factorial_c programs/factorial.c
./build/factorial_c
```

### 3. Run the Assembly Version

```bash
# Assemble
./build/assembler programs/factorial.asm /tmp/factorial.bin

# Execute
./build/emulator /tmp/factorial.bin
```

**Output:**
```
=== Starting Execution ===
F(5) = 120

=== Execution Complete ===
Instructions executed: 90
```

### 4. View Detailed Execution Trace

```bash
./build/emulator /tmp/factorial.bin -d
```

This shows:
- Each instruction executed
- Register values after each step
- Stack pointer changes during recursion
- Memory addresses and program counter

### 5. Run Interactive Demo

```bash
./demo.sh
```

This presents a step-by-step walkthrough of:
1. C program with recursion
2. Assembly translation
3. Memory layout diagram
4. Function call mechanism
5. Recursion trace with stack visualization
6. Actual execution trace


## CPU Architecture

### Registers
- **R0**: Return value / Accumulator
- **R1**: Function argument (n)
- **R2**: Temporary storage
- **R3-R7**: General purpose
- **SP**: Stack pointer (hardware managed)
- **PC**: Program counter (hardware managed)

### Memory Map (64KB Address Space)
- **0x0000-0x7FFF**: Code section (32KB)
- **0x8000-0xEFFF**: Data section (28KB)
- **0xF000-0xF0FF**: Memory-mapped I/O (256 bytes)
- **0xF100-0xFFFF**: Stack (3.75KB, grows downward)

### Instruction Set (Subset Used)
- **MOVI**: Move immediate to register
- **MOV**: Move register to register
- **PUSH/POP**: Stack operations
- **CALL/RET**: Function calls
- **ADD/SUB/MUL/DIV**: Arithmetic
- **CMPI**: Compare immediate
- **JZ/JNZ**: Conditional jumps
- **STORE**: Memory-mapped I/O output
- **HALT**: Stop execution



