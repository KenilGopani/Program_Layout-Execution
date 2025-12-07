# Recursive Factorial Program - Assembly Implementation

## Assignment Overview

This project demonstrates a recursive factorial calculation implemented in both C and assembly language, running on a custom 16-bit RISC CPU emulator. The assignment showcases:

1. **C recursive function** with a main driver program
2. **Executable memory layout** in the virtual CPU
3. **Function call mechanism** (CALL/RET instructions)
4. **Recursion execution** with stack frame management

## Project Structure

```
software-cpu-design/
├── programs/
│   ├── factorial.c          # C implementation with recursion
│   ├── factorial.asm        # Assembly translation
│   └── factorial_test.asm   # Test version
├── docs/
│   ├── factorial_memory_layout.md  # Detailed memory analysis
│   └── cpu_schematic.md            # CPU architecture
├── src/
│   ├── assembler/          # Assembler (converts .asm to binary)
│   ├── emulator/           # CPU emulator
│   └── common/             # Shared definitions
├── build/                  # Compiled binaries
├── ASSIGNMENT_SOLUTION.md  # Complete assignment writeup
├── C_TO_ASSEMBLY.md       # C-to-assembly translation guide
├── demo.sh                # Interactive demonstration script
└── README_FACTORIAL.md    # This file
```

## Quick Start

### 1. Build the Tools

```bash
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

**Output:**
```
F(5) = 120
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

## Implementation Details

### C Program (`programs/factorial.c`)

```c
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);  // Recursive call
}

int main() {
    int result = factorial(5);
    printf("F(%d) = %d\n", 5, result);
    return 0;
}
```

### Assembly Program (`programs/factorial.asm`)

Key sections:

**Main Program:**
```assembly
START:
    MOVI R1, 5          ; Load argument n=5
    CALL FACTORIAL      ; Call recursive function
    ; ... print result ...
    HALT
```

**Recursive Function:**
```assembly
FACTORIAL:
    PUSH R1             ; Save argument
    CMPI R1, 1          ; Check base case
    JZ BASE_CASE        ; If n<=1, goto base case
    
    PUSH R1             ; Save n for multiplication
    SUBI R1, R1, 1      ; n = n - 1
    CALL FACTORIAL      ; Recursive call
    POP R2              ; Restore n
    MUL R0, R0, R2      ; result = result * n
    
    POP R1
    RET

BASE_CASE:
    MOVI R0, 1          ; Return 1
    POP R1
    RET
```

## Assignment Requirements Fulfilled

### ✅ 1. C Function with Recursion
- **File:** `programs/factorial.c`
- **Function:** `int factorial(int n)`
- **Recursion:** Calls itself with `n-1`

### ✅ 2. Main Driver Program
- **File:** `programs/factorial.c`
- **Function:** `int main()`
- **Purpose:** Calls `factorial(5)` and displays result

### ✅ 3. Executable Memory Layout
**Demonstrated in debug output and documentation:**

```
Memory Address Range    | Section              | Contents
------------------------+----------------------+--------------------------------
0x0000 - 0x0031        | Main Program Code    | START label
                       |                      | - Print "F(5) = "
                       |                      | - MOVI R1, 5
                       |                      | - CALL FACTORIAL (at 0x0032)
                       |                      | - Print result
                       |                      | - HALT
------------------------+----------------------+--------------------------------
0x0072 - 0x008E        | Function Code        | FACTORIAL label
                       |                      | - PUSH R1 (prologue)
                       |                      | - CMPI R1, 1 (base case check)
                       |                      | - JZ BASE_CASE
                       |                      | - PUSH R1 (save n)
                       |                      | - SUBI R1, R1, 1
                       |                      | - CALL FACTORIAL (recursive)
                       |                      | - POP R2 (restore n)
                       |                      | - MUL R0, R0, R2
                       |                      | - POP R1 (epilogue)
                       |                      | - RET
                       |                      | BASE_CASE:
                       |                      | - MOVI R0, 1
                       |                      | - POP R1
                       |                      | - RET
------------------------+----------------------+--------------------------------
0xF000                 | Memory-Mapped I/O    | Console output register
------------------------+----------------------+--------------------------------
0xF100 - 0xFFFF        | Stack Section        | Stack (grows downward)
                       |                      | Initial SP: 0xFFFF
                       |                      | Deepest: 0xFFE3 (during fact(1))
```

### ✅ 4. Function Call Mechanism

**CALL Instruction (0x0032):**
1. Pushes return address (0x0034) onto stack
2. Sets PC = 0x0072 (FACTORIAL address)
3. SP decrements: 0xFFFF → 0xFFFD

**RET Instruction (0x0088):**
1. Pops return address from stack into PC
2. SP increments back: 0xFFFD → 0xFFFF
3. Execution continues at caller

**Visible in debug output:**
```
[18] 0x0032: CALL 0x0072
     R1=0x0005  SP=0xfffd  ← Stack grows
     
[64] 0x0088: RET
     R0=0x0078  SP=0xffff  ← Stack restored
```

### ✅ 5. Recursion Execution

**5 Recursive Calls (Going Deeper):**
```
Call 1: factorial(5)  R1=5  SP=0xFFFD
Call 2: factorial(4)  R1=4  SP=0xFFF7
Call 3: factorial(3)  R1=3  SP=0xFFF1
Call 4: factorial(2)  R1=2  SP=0xFFEB
Call 5: factorial(1)  R1=1  SP=0xFFE5  ← Deepest
```

**Base Case:**
```
MOVI R0, 1  (when R1=1)
```

**Unwinding with Multiplications:**
```
Return 1: 1 × 2 = 2    (SP=0xFFE9)
Return 2: 2 × 3 = 6    (SP=0xFFEF)
Return 3: 6 × 4 = 24   (SP=0xFFF5)
Return 4: 24 × 5 = 120 (SP=0xFFFB)
Return 5: Back to main (SP=0xFFFF)
```

**Final Result:** R0 = 0x0078 (120 decimal) ✅

### Detailed Stack State at Deepest Point

When factorial(1) is executing (deepest recursion):

```
Memory Address  | Content                    | Description
────────────────┼────────────────────────────┼────────────────────────
0xFFFF          | (unused)                   | Stack top
0xFFFD          | Return addr to main        | main's CALL
0xFFFB          | 5 (saved R1)              | factorial(5) prologue
0xFFF9          | 5 (saved n)               | factorial(5) before recursion
0xFFF7          | Return addr to fact(5)     | fact(5)'s CALL
0xFFF5          | 4 (saved R1)              | factorial(4) prologue
0xFFF3          | 4 (saved n)               | factorial(4) before recursion
0xFFF1          | Return addr to fact(4)     | fact(4)'s CALL
0xFFEF          | 3 (saved R1)              | factorial(3) prologue
0xFFED          | 3 (saved n)               | factorial(3) before recursion
0xFFEB          | Return addr to fact(3)     | fact(3)'s CALL
0xFFE9          | 2 (saved R1)              | factorial(2) prologue
0xFFE7          | 2 (saved n)               | factorial(2) before recursion
0xFFE5          | Return addr to fact(2)     | fact(2)'s CALL
0xFFE3          | 1 (saved R1)              | factorial(1) prologue ← SP
────────────────┴────────────────────────────┴────────────────────────

Total stack usage: 28 bytes (14 words)
Stack frames: 5 (one per recursive call)
```

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

## Key Features Demonstrated

### Stack Frame Management
Each recursive call creates a stack frame containing:
1. **Return address** (pushed by CALL)
2. **Saved R1** (function argument)
3. **Saved n** (for multiplication after recursive call)

**Stack Frame Structure:**
```
Higher Memory (0xFFFF)
    │
    ├─────────────────────────┐
    │   Previous Stack Frame  │
    ├─────────────────────────┤ ← SP before CALL
    │   Return Address        │ (Pushed by CALL)
    ├─────────────────────────┤ ← SP after CALL
    │   Saved R1 (argument)   │ (Pushed by prologue)
    ├─────────────────────────┤ ← Current SP
    │   Saved n (for MUL)     │ (Pushed before recursive call)
    └─────────────────────────┘
    │
Lower Memory
```

**Maximum Stack Usage:**
- Stack depth: ~30 bytes for factorial(5)
- Deepest point: SP = 0xFFE3 (when factorial(1) is called)
- Stack grows: 0xFFFF → 0xFFE3 (28 bytes)
- Stack restores: 0xFFE3 → 0xFFFF (back to initial)

### Register Usage Convention
- **Caller-saved**: R0 (return value)
- **Callee-saved**: R1 (restored in function epilogue)
- **Temporary**: R2 (restored from stack when needed)

### Two-Pass Assembly
1. **Pass 1**: Build symbol table (labels → addresses)
2. **Pass 2**: Generate machine code with resolved addresses

## Documentation

- **ASSIGNMENT_SOLUTION.md**: Complete assignment writeup with all requirements
- **C_TO_ASSEMBLY.md**: Line-by-line C-to-assembly translation
- **docs/factorial_memory_layout.md**: Detailed memory layout and execution trace
- **docs/cpu_schematic.md**: CPU architecture specification
- **docs/isa_specification.md**: Instruction set architecture

## Testing

All test cases pass:
```bash
factorial(1) = 1   ✓
factorial(2) = 2   ✓
factorial(3) = 6   ✓
factorial(4) = 24  ✓
factorial(5) = 120 ✓
factorial(6) = 720 ✓
```

## Debugging

### Debug Mode
```bash
./build/emulator /tmp/factorial.bin -d
```
Shows:
- Instruction number and address
- Instruction disassembly
- Register values after execution
- Flags (Zero, Carry, Negative, Overflow)

### Memory Dump
```bash
./build/emulator /tmp/factorial.bin -m
```
Shows memory contents after execution.

## Bug Fix History

**Issue Found**: PUSH/POP instructions always used R0 regardless of specified register.

**Root Cause**: Assembler encoded both PUSH and POP using Rd field (bits 9-7), but CPU reads PUSH from Rs field (bits 6-4).

**Fix**: Modified `src/assembler/assembler.cpp` lines 393-442:
- PUSH now uses: `MAKE_INSTR(opcode, 0, rs, 0)` (Rs field)
- POP continues using: `MAKE_INSTR(opcode, rd, 0, 0)` (Rd field)

**Result**: All recursive factorial tests now pass correctly.

## Conclusion

This project successfully demonstrates:
1. ✅ **Recursive programming** in C and assembly
2. ✅ **Memory organization** in a virtual CPU
3. ✅ **Function call conventions** with stack-based calling
4. ✅ **Recursion mechanics** with visible stack growth/unwinding
5. ✅ **Low-level execution** with instruction-by-instruction tracing

The implementation proves that high-level concepts (recursion, function calls) map directly to low-level machine operations (CALL/RET, PUSH/POP, stack pointer management).

## Author

Assignment completed for Software CPU Design course.

## License

Educational project - see course guidelines.
