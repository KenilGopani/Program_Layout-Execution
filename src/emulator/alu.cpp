/**
 * Arithmetic Logic Unit (ALU) Implementation
 * 
 * This file implements arithmetic and logical operations for the CPU.
 * All operations update condition flags (Zero, Negative, Carry, Overflow)
 * appropriately based on the result.
 * 
 * Condition Flags:
 *   - Zero (Z): Set when result is zero
 *   - Negative (N): Set when result is negative (MSB = 1)
 *   - Carry (C): Set on unsigned overflow or borrow
 *   - Overflow (V): Set on signed overflow
 */

#include "alu.h"

void ALU::clear_flags(word_t &flags) { flags = 0; }

/**
 * Set zero flag if result equals zero
 */
void ALU::set_zero_flag(word_t result, word_t &flags) {
  if (result == 0) {
    flags |= FLAG_ZERO;
  }
}

/**
 * Set negative flag if MSB (bit 15) is set
 * In two's complement, MSB=1 indicates negative number
 */
void ALU::set_negative_flag(word_t result, word_t &flags) {
  if (result & 0x8000) {
    flags |= FLAG_NEGATIVE;
  }
}

/**
 * Perform addition with carry and overflow detection
 */
word_t ALU::add(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  // Use 32-bit arithmetic to detect overflow
  uint32_t result32 = (uint32_t)a + (uint32_t)b;
  word_t result = (word_t)result32;

  // Carry flag: unsigned overflow
  if (result32 > 0xFFFF) {
    flags |= FLAG_CARRY;
  }

  // Overflow flag: signed overflow
  // Occurs when adding two same-sign numbers produces opposite sign
  bool a_neg = (a & 0x8000) != 0;
  bool b_neg = (b & 0x8000) != 0;
  bool r_neg = (result & 0x8000) != 0;

  if ((a_neg == b_neg) && (a_neg != r_neg)) {
    flags |= FLAG_OVERFLOW;
  }

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

/**
 * Perform subtraction with borrow and overflow detection
 * 
 */
word_t ALU::sub(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  int32_t result32 = (int32_t)a - (int32_t)b;
  word_t result = (word_t)result32;

  // Carry flag: set when borrow is needed
  if (a < b) {
    flags |= FLAG_CARRY;
  }

  // Overflow flag: signed subtraction overflow
  // Overflow occurs when: (+) - (-) = (-) or (-) - (+) = (+)
  bool a_neg = (a & 0x8000) != 0;
  bool b_neg = (b & 0x8000) != 0;
  bool r_neg = (result & 0x8000) != 0;

  if ((a_neg != b_neg) && (a_neg != r_neg)) {
    flags |= FLAG_OVERFLOW;
  }

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Multiplication (lower 16 bits only)
word_t ALU::mul(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  uint32_t result32 = (uint32_t)a * (uint32_t)b;
  word_t result = (word_t)(result32 & 0xFFFF);

  // Set carry if result doesn't fit in 16 bits
  if (result32 > 0xFFFF) {
    flags |= FLAG_CARRY;
  }

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Division (integer division)
word_t ALU::div(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  // Division by zero
  if (b == 0) {
    flags |= FLAG_OVERFLOW; // Use overflow to indicate error
    return 0xFFFF;          // Return max value on error
  }

  word_t result = a / b;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Bitwise AND
word_t ALU::and_op(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  word_t result = a & b;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Bitwise OR
word_t ALU::or_op(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  word_t result = a | b;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Bitwise XOR
word_t ALU::xor_op(word_t a, word_t b, word_t &flags) {
  clear_flags(flags);

  word_t result = a ^ b;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Bitwise NOT
word_t ALU::not_op(word_t a, word_t &flags) {
  clear_flags(flags);

  word_t result = ~a;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Shift left (logical)
word_t ALU::shl(word_t a, word_t shift, word_t &flags) {
  clear_flags(flags);

  if (shift >= 16) {
    // Shift all bits out
    if (shift == 16 && (a & 0x0001)) {
      flags |= FLAG_CARRY; // Last bit shifted out
    }
    set_zero_flag(0, flags);
    return 0;
  }

  // Check if MSB will be shifted out
  if (shift > 0 && (a & (1 << (16 - shift)))) {
    flags |= FLAG_CARRY;
  }

  word_t result = a << shift;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Shift right (logical)
word_t ALU::shr(word_t a, word_t shift, word_t &flags) {
  clear_flags(flags);

  if (shift >= 16) {
    // Shift all bits out
    if (shift == 16 && (a & 0x8000)) {
      flags |= FLAG_CARRY; // Last bit shifted out
    }
    set_zero_flag(0, flags);
    return 0;
  }

  // Check if LSB will be shifted out
  if (shift > 0 && (a & (1 << (shift - 1)))) {
    flags |= FLAG_CARRY;
  }

  word_t result = a >> shift;

  set_zero_flag(result, flags);
  set_negative_flag(result, flags);

  return result;
}

// Compare (performs subtraction but only sets flags)
word_t ALU::compare(word_t a, word_t b, word_t &flags) {
  sub(a, b, flags); // Perform subtraction to set flags
  return 0;         // Don't return result for comparison
}
