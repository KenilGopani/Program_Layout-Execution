; Recursive Factorial Program
; Demonstrates function calls, recursion, and stack frame management
; 
; C equivalent:
;   int factorial(int n) {
;       if (n <= 1) return 1;
;       return n * factorial(n - 1);
;   }
;   
;   int main() {
;       int result = factorial(5);
;       // Output result to console
;       return 0;
;   }

; Memory Layout:
; 0x0000-0x7FFF: Program Code (this file)
; 0x8000-0xEFFF: Data Section
; 0xF000-0xF0FF: Memory-Mapped I/O
; 0xF100-0xFFFF: Stack (grows downward from 0xFFFF)

; Register Usage Convention:
; R0: Return value / accumulator
; R1: Function argument (n)
; R2: Temporary storage / saved n for multiplication
; R3: I/O and temporary
; R4-R7: General purpose

START:
    ; Initialize stack pointer (handled by CPU reset)
    ; SP starts at 0xFFFF
    
    ; Print 'F' for Factorial
    MOVI R3, 60
    ADDI R3, R3, 7
    ADDI R3, R3, 3        ; 'F' = 70
    STORE R3, 0xF000
    
    ; Print '('
    MOVI R3, 40           ; '(' = 40
    STORE R3, 0xF000
    
    ; Print '5'
    MOVI R3, 48
    ADDI R3, R3, 5        ; '5' = 53
    STORE R3, 0xF000
    
    ; Print ')'
    MOVI R3, 41           ; ')' = 41
    STORE R3, 0xF000
    
    ; Print ' '
    MOVI R3, 32           ; ' ' = 32
    STORE R3, 0xF000
    
    ; Print '='
    MOVI R3, 61           ; '=' = 61
    STORE R3, 0xF000
    
    ; Print space
    MOVI R3, 32           ; ' ' = 32
    STORE R3, 0xF000
    
    ; Call factorial(5)
    MOVI R1, 5            ; Argument: n = 5
    CALL FACTORIAL        ; Result will be in R0
    
    ; Store result for output (R0 = 120)
    MOV R4, R0            ; Save result to R4
    
    ; Output result as decimal number (3 digits)
    ; Extract hundreds digit
    MOVI R5, 50
    MOVI R6, 50
    ADD R5, R5, R6        ; R5 = 100
    DIV R6, R4, R5        ; R6 = result / 100
    MUL R7, R6, R5        ; R7 = (result / 100) * 100
    SUB R4, R4, R7        ; R4 = result % 100
    
    ; Print hundreds digit (skip if 0)
    CMPI R6, 0
    JZ SKIP_HUNDREDS
    MOVI R3, 48
    ADD R3, R3, R6        ; Convert to ASCII
    STORE R3, 0xF000
SKIP_HUNDREDS:
    
    ; Extract tens digit
    MOVI R5, 10
    DIV R6, R4, R5        ; R6 = remaining / 10
    MUL R7, R6, R5        ; R7 = (remaining / 10) * 10
    SUB R4, R4, R7        ; R4 = remaining % 10
    
    ; Print tens digit
    MOVI R3, 48
    ADD R3, R3, R6        ; Convert to ASCII
    STORE R3, 0xF000
    
    ; Print ones digit
    MOVI R3, 48
    ADD R3, R3, R4        ; Convert to ASCII
    STORE R3, 0xF000
    
    ; Print newline
    MOVI R3, 10           ; '\n' = 10
    STORE R3, 0xF000
    
    ; Exit program
    HALT

; ============================================================================
; FACTORIAL Function - Recursive Implementation
; ============================================================================
; Input:  R1 = n (the number to compute factorial of)
; Output: R0 = n! (factorial result)
; 
; Stack Frame Layout (per call):
; [SP+4]: Return address (saved by CALL)
; [SP+2]: Saved R1 (argument n)
; [SP+0]: Current SP after prologue
; ============================================================================
FACTORIAL:
    ; Function Prologue: Save registers and setup stack frame
    PUSH R1                 ; Save argument (n)
    
    ; Base case: if (n <= 1) return 1
    CMPI R1, 1              ; Compare n with 1
    JZ BASE_CASE            ; If n == 1, goto base case
    
    ; Recursive case: n * factorial(n-1)
    ; Save n because we'll need it for multiplication later
    ; Can't use R2 because CALL will clobber it
    PUSH R1                 ; Push n onto stack again for later
    
    ; Prepare argument for recursive call
    SUBI R1, R1, 1          ; R1 = n - 1 (argument for recursive call)
    CALL FACTORIAL          ; Recursive call: R0 = factorial(n-1)
    
    ; Pop the saved n value into R2 for multiplication
    POP R2                  ; R2 = n (the value we saved before CALL)
    
    ; Now multiply: R0 = n * factorial(n-1)
    MUL R0, R0, R2          ; R0 = factorial(n-1) * n
    
    ; Function Epilogue: Restore registers and return
    POP R1                  ; Restore original R1
    RET                     ; Return to caller

BASE_CASE:
    ; Return 1 for base case
    MOVI R0, 1              ; Return value = 1
    POP R1                  ; Restore R1
    RET                     ; Return to caller
