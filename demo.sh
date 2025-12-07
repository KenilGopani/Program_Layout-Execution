#!/bin/bash
# Factorial Program Demo - Shows Memory Layout, Function Calls, and Recursion

cd "$(dirname "$0")"

echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║            FACTORIAL PROGRAM - ASSIGNMENT DEMONSTRATION              ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""

# 1. Show C Program
echo "═══ 1. C PROGRAM WITH RECURSION ═══"
echo ""
cat << 'EOF'
// File: programs/factorial.c

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);  // ← Recursive call
}

int main() {
    int result = factorial(5);
    printf("F(%d) = %d\n", 5, result);
    return 0;
}
EOF
echo ""
echo "Compile and run C version:"
echo "  $ gcc -o build/factorial_c programs/factorial.c && ./build/factorial_c"
./build/factorial_c
echo ""
read -p "Press Enter to continue..."
clear

# 2. Show Assembly Translation
echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║                    ASSEMBLY TRANSLATION                              ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""
echo "═══ 2. ASSEMBLY PROGRAM (excerpt) ═══"
echo ""
cat << 'EOF'
// File: programs/factorial.asm

START:
    MOVI R1, 5            ; Load argument n=5
    CALL FACTORIAL        ; Call recursive function
    ; ... print result ...
    HALT

FACTORIAL:
    PUSH R1               ; Save argument
    CMPI R1, 1           ; Check base case
    JZ BASE_CASE         ; If n<=1, jump to base
    
    PUSH R1              ; Save n for multiplication
    SUBI R1, R1, 1       ; n = n - 1
    CALL FACTORIAL       ; Recursive call
    POP R2               ; Restore n
    MUL R0, R0, R2       ; result = result * n
    
    POP R1               ; Restore R1
    RET                  ; Return to caller

BASE_CASE:
    MOVI R0, 1           ; Return 1
    POP R1
    RET
EOF
echo ""
echo "Assemble and run assembly version:"
echo "  $ ./build/assembler programs/factorial.asm /tmp/factorial.bin"
./build/assembler programs/factorial.asm /tmp/factorial.bin 2>&1 | grep -E "Assembling|Successfully"
echo "  $ ./build/emulator /tmp/factorial.bin"
./build/emulator /tmp/factorial.bin 2>&1 | grep -A 1 "Starting Execution"
echo ""
read -p "Press Enter to continue..."
clear

# 3. Show Memory Layout
echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║                      MEMORY LAYOUT                                   ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""
echo "═══ 3. EXECUTABLE MEMORY LAYOUT ═══"
echo ""
cat << 'EOF'
┌──────────────────┬────────────────┬─────────────────────────────┐
│ Address Range    │ Section        │ Contents                    │
├──────────────────┼────────────────┼─────────────────────────────┤
│ 0x0000 - 0x0031  │ Main Code      │ START label                 │
│                  │                │ - Print "F(5) = "           │
│ 0x0032           │                │ - MOVI R1, 5                │
│                  │                │ - CALL FACTORIAL            │
│ 0x0034 - 0x0071  │                │ - Print result              │
│                  │                │ - HALT                      │
├──────────────────┼────────────────┼─────────────────────────────┤
│ 0x0072 - 0x008E  │ Function Code  │ FACTORIAL function          │
│                  │                │ - Prologue (PUSH R1)        │
│                  │                │ - Base case check           │
│                  │                │ - Recursive call            │
│                  │                │ - Multiplication            │
│                  │                │ - Epilogue (POP R1, RET)    │
├──────────────────┼────────────────┼─────────────────────────────┤
│ 0xF000           │ Memory-Mapped  │ Console output register     │
│                  │ I/O            │                             │
├──────────────────┼────────────────┼─────────────────────────────┤
│ 0xFFFF (top)     │ Stack          │ Grows downward ↓            │
│ 0xFFFD           │                │ After 1st CALL              │
│ 0xFFF7           │                │ After 2nd CALL (fact(4))    │
│ 0xFFF1           │                │ After 3rd CALL (fact(3))    │
│ 0xFFEB           │                │ After 4th CALL (fact(2))    │
│ 0xFFE5           │                │ After 5th CALL (fact(1))    │
│ 0xFFE3           │                │ ← Deepest point             │
└──────────────────┴────────────────┴─────────────────────────────┘

Key Memory Addresses:
  • Code starts at 0x0000
  • FACTORIAL function at 0x0072
  • I/O output at 0xF000
  • Stack starts at 0xFFFF, grows to 0xFFE3 during recursion
EOF
echo ""
read -p "Press Enter to continue..."
clear

# 4. Show Function Call Mechanism
echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║                   FUNCTION CALL MECHANISM                            ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""
echo "═══ 4. HOW FUNCTION CALLS ARE HANDLED ═══"
echo ""
cat << 'EOF'
CALL Instruction (at 0x0032):
┌─────────────────────────────────────────────────────────────┐
│ BEFORE CALL:                                                │
│   PC = 0x0032  (at CALL instruction)                        │
│   SP = 0xFFFF  (stack empty)                                │
│   R1 = 5       (function argument)                          │
│                                                              │
│ CALL EXECUTION:                                             │
│   1. Push return address (PC+2 = 0x0034) onto stack        │
│   2. Set PC = 0x0072 (FACTORIAL address)                    │
│   3. SP = SP - 2 = 0xFFFD                                   │
│                                                              │
│ AFTER CALL:                                                 │
│   PC = 0x0072  (at FACTORIAL entry)                         │
│   SP = 0xFFFD  (stack has return address)                   │
│   Stack[0xFFFF] = 0x0034 (return address saved)            │
└─────────────────────────────────────────────────────────────┘

RET Instruction (at 0x0088):
┌─────────────────────────────────────────────────────────────┐
│ BEFORE RET:                                                 │
│   PC = 0x0088  (at RET instruction)                         │
│   SP = 0xFFFD  (points to return address)                   │
│   R0 = 120     (return value)                               │
│                                                              │
│ RET EXECUTION:                                              │
│   1. Pop return address from stack into PC                  │
│   2. SP = SP + 2 = 0xFFFF                                   │
│                                                              │
│ AFTER RET:                                                  │
│   PC = 0x0034  (back to caller)                             │
│   SP = 0xFFFF  (stack restored)                             │
│   R0 = 120     (result available to caller)                 │
└─────────────────────────────────────────────────────────────┘

Register Convention:
  • R0: Return value
  • R1: Function argument (n)
  • R2: Temporary (saved n for multiplication)
  • SP: Stack pointer (hardware managed)
  • PC: Program counter (hardware managed)
EOF
echo ""
read -p "Press Enter to continue..."
clear

# 5. Show Recursion Trace
echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║                      RECURSION EXECUTION                             ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""
echo "═══ 5. HOW RECURSION IS CARRIED OUT ═══"
echo ""
cat << 'EOF'
RECURSIVE CALLS (Going Deeper):
┌────────┬─────┬──────────┬────────────────────────────────────┐
│ Step   │ R1  │ SP       │ Action                             │
├────────┼─────┼──────────┼────────────────────────────────────┤
│ Call 1 │ 5   │ 0xFFFD   │ main() calls factorial(5)          │
│ Call 2 │ 4   │ 0xFFF7   │ factorial(5) calls factorial(4)    │
│ Call 3 │ 3   │ 0xFFF1   │ factorial(4) calls factorial(3)    │
│ Call 4 │ 2   │ 0xFFEB   │ factorial(3) calls factorial(2)    │
│ Call 5 │ 1   │ 0xFFE5   │ factorial(2) calls factorial(1)    │
└────────┴─────┴──────────┴────────────────────────────────────┘
                            ↓
┌────────────────────────────────────────────────────────────────┐
│ BASE CASE REACHED (R1 = 1, SP = 0xFFE3):                      │
│   MOVI R0, 1    → Sets return value to 1                      │
│   RET           → Starts unwinding                            │
└────────────────────────────────────────────────────────────────┘
                            ↓
UNWINDING (Coming Back):
┌────────┬─────┬─────┬──────────┬────────────────────────────┐
│ Return │ R0  │ R2  │ SP       │ Calculation                │
├────────┼─────┼─────┼──────────┼────────────────────────────┤
│ Ret 1  │ 1   │ 2   │ 0xFFE9   │ 1 × 2 = 2                  │
│ Ret 2  │ 2   │ 3   │ 0xFFEF   │ 2 × 3 = 6                  │
│ Ret 3  │ 6   │ 4   │ 0xFFF5   │ 6 × 4 = 24                 │
│ Ret 4  │ 24  │ 5   │ 0xFFFB   │ 24 × 5 = 120               │
│ Ret 5  │ 120 │ -   │ 0xFFFF   │ Back to main, result ready │
└────────┴─────┴─────┴──────────┴────────────────────────────┘

Stack Pointer Journey:
  0xFFFF → 0xFFFD → 0xFFF7 → 0xFFF1 → 0xFFEB → 0xFFE5 (deepest)
          ↓ Going down (5 recursive calls)
          
  0xFFE5 → 0xFFE9 → 0xFFEF → 0xFFF5 → 0xFFFB → 0xFFFF (restored)
          ↑ Unwinding (5 returns with multiplications)

Final Result: R0 = 120 (5! = 5 × 4 × 3 × 2 × 1)
EOF
echo ""
read -p "Press Enter to see actual execution trace..."
clear

# 6. Show Actual Debug Output
echo "╔══════════════════════════════════════════════════════════════════════╗"
echo "║              ACTUAL EXECUTION TRACE (First 20 steps)                ║"
echo "╚══════════════════════════════════════════════════════════════════════╝"
echo ""
echo "Running: ./build/emulator /tmp/factorial.bin -d"
echo ""
./build/emulator /tmp/factorial.bin -d 2>&1 | grep -E "\[1[78]\]|\[2[0-9]\]|\[3[0-9]\]|\[4[0-9]\]|\[5[0-9]\]" | head -25
echo ""