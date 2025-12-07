/**
 * Common Type Definitions and Constants
 * 
 * This header defines fundamental types, memory layout, and instruction
 * format macros used throughout the assembler and emulator.
 * 
 */

#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>

// Basic Type Definitions

typedef uint16_t word_t; // 16-bit word - primary data unit
typedef uint8_t byte_t;  // 8-bit byte - for memory addressing
typedef uint16_t addr_t; // 16-bit address - 64KB address space


// Memory Map Configuration

const size_t MEMORY_SIZE = 0x10000;  // 64KB total addressable memory
const addr_t PROGRAM_START = 0x0000; // Code segment: executable instructions
const addr_t PROGRAM_END = 0x7FFF;   // 32KB for program code
const addr_t DATA_START = 0x8000;    // Data segment: variables and constants
const addr_t DATA_END = 0xEFFF;      // 28KB for data storage
const addr_t IO_START = 0xF000;      // I/O region: memory-mapped devices
const addr_t IO_END = 0xF0FF;        // 256 bytes for I/O
const addr_t STACK_START = 0xF100;   // Stack segment: function calls/locals
const addr_t STACK_END = 0xFFFF;     // Stack grows downward from top


// Memory-Mapped I/O Addresses

const addr_t IO_CONSOLE_OUT = 0xF000; // Write byte to console output
const addr_t IO_CONSOLE_IN = 0xF001;  // Read byte from console input
const addr_t IO_TIMER_CTRL = 0xF002;  // Timer control register
const addr_t IO_TIMER_VAL = 0xF003;   // Timer value register


// CPU Architecture Parameters

const int NUM_REGISTERS = 8; // R0-R7: general-purpose registers


// Condition Flag Bits

const word_t FLAG_ZERO = 0x0001;     // Z: Result was zero
const word_t FLAG_CARRY = 0x0002;    // C: Unsigned overflow or borrow
const word_t FLAG_NEGATIVE = 0x0004; // N: Result was negative (MSB=1)
const word_t FLAG_OVERFLOW = 0x0008; // V: Signed overflow occurred


// Instruction Format Decoding Macros

#define GET_OPCODE(instr) (((instr) >> 10) & 0x3F)  // Extract 6-bit opcode
#define GET_RD(instr) (((instr) >> 7) & 0x07)       // Extract destination register
#define GET_RS(instr) (((instr) >> 4) & 0x07)       // Extract source register 1
#define GET_RT(instr) ((instr) & 0x0F)              // Extract source register 2 or immediate
#define GET_IMM4(instr) ((instr) & 0x0F)            // Extract 4-bit immediate
#define GET_IMM7(instr) ((instr) & 0x7F)            // Extract 7-bit immediate
#define GET_IMM10(instr) ((instr) & 0x3FF)          // Extract 10-bit immediate

// Instruction Encoding Macros
#define MAKE_INSTR(op, rd, rs, rt)                                             \
  ((((op) & 0x3F) << 10) | (((rd) & 0x07) << 7) | (((rs) & 0x07) << 4) |       \
   ((rt) & 0x0F))

#define MAKE_INSTR_IMM7(op, rd, imm)                                           \
  ((((op) & 0x3F) << 10) | (((rd) & 0x07) << 7) | ((imm) & 0x7F))

// Sign Extension Functions
// Convert unsigned immediate values to signed 16-bit integers

/**
 * Sign-extend 4-bit immediate to 16-bit signed value
 */
inline int16_t sign_extend_4bit(word_t val) {
  if (val & 0x08)  // If bit 3 is set (negative)
    return (int16_t)(val | 0xFFF0);
  return (int16_t)val;
}

/**
 * Sign-extend 7-bit immediate to 16-bit signed value
 */
inline int16_t sign_extend_7bit(word_t val) {
  if (val & 0x40)  // If bit 6 is set (negative)
    return (int16_t)(val | 0xFF80);
  return (int16_t)val;
}

/**
 * Sign-extend 10-bit immediate to 16-bit signed value
 */
inline int16_t sign_extend_10bit(word_t val) {
  if (val & 0x200)  // If bit 9 is set (negative)
    return (int16_t)(val | 0xFC00);
  return (int16_t)val;
}

#endif // TYPES_H
