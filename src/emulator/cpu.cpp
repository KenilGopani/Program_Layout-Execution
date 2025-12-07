/**
 * CPU Implementation
 * 
 * This file implements the core CPU emulator including:
 *   - Fetch-decode-execute cycle
 *   - Register file management
 *   - Stack operations for function calls
 *   - Instruction execution for all opcodes
 *   - Condition flag updates
 */

#include "cpu.h"
#include <iomanip>
#include <iostream>

CPU::CPU(Memory &mem) : memory(mem) { reset(); }


void CPU::reset() {
  // Clear all general-purpose registers
  for (int i = 0; i < NUM_REGISTERS; i++) {
    registers[i] = 0;
  }

  pc = PROGRAM_START;    // Start executing from address 0x0000
  sp = STACK_END;        // Stack grows downward from top of memory
  flags = 0;             // Clear all condition flags
  halted = false;        // CPU is ready to run
  debug_mode = false;
  instruction_count = 0;
}

word_t CPU::get_register(int reg) const {
  if (reg >= 0 && reg < NUM_REGISTERS) {
    return registers[reg];
  }
  return 0;
}

/**
 * Push a value onto the stack
 * Stack grows downward, so we decrement SP before writing
 */
void CPU::push(word_t value) {
  sp -= 2; // Each word is 2 bytes
  memory.write_word(sp, value);
}

/**
 * Pop a value from the stack
 * Read the value then increment SP
 */
word_t CPU::pop() {
  word_t value = memory.read_word(sp);
  sp += 2;
  return value;
}

void CPU::halt() { halted = true; }

/**
 * Execute program until CPU halts
 */
void CPU::run() {
  while (!halted) {
    step();
  }
}

/**
 * Execute a single instruction
 */
void CPU::step() {
  if (halted)
    return;

  fetch_decode_execute();
  instruction_count++;
}

/**
 * The heart of the CPU: Fetch-Decode-Execute cycle
 * 
 * 1. FETCH: Read instruction from memory at PC
 * 2. DECODE: Extract opcode and operands from instruction
 * 3. EXECUTE: Perform the operation
 */
void CPU::fetch_decode_execute() {
  // FETCH: Read the next instruction from memory
  word_t instruction = memory.read_word(pc);
  addr_t current_pc = pc;
  pc += 2; // Move to next instruction (each instruction is 2 bytes)

  // Display instruction in debug mode
  if (debug_mode) {
    std::cout << "\n[" << instruction_count << "] ";
    disassemble_instruction(instruction, current_pc);
    std::cout << std::endl;
  }

  // DECODE & EXECUTE: Process the instruction
  execute_instruction(instruction);

  // Show updated register state in debug mode
  if (debug_mode) {
    print_registers();
    print_flags();
  }
}


void CPU::execute_instruction(word_t instruction) {
  // DECODE: Extract fields from the 16-bit instruction
  byte_t opcode = GET_OPCODE(instruction);
  byte_t rd = GET_RD(instruction);
  byte_t rs = GET_RS(instruction);
  byte_t rt = GET_RT(instruction);
  byte_t imm4 = GET_IMM4(instruction);
  byte_t imm7 = GET_IMM7(instruction);

  // EXECUTE: Perform operation based on opcode
  switch (opcode) {
  // Data Movement Instructions
  case OP_NOP:
    // No operation (or register move when Rd != Rs)
    if (rd != rs) {
      registers[rd] = registers[rs];
    }
    break;

  case OP_MOVI:
    // Move immediate value into register (sign-extended)
    registers[rd] = sign_extend_7bit(imm7);
    break;

  case OP_LOAD_IND:
    // Load word from memory address in Rs into Rd
    registers[rd] = memory.read_word(registers[rs]);
    break;

  case OP_LOAD_DIR: {
    // Load from direct address (next word)
    word_t address = memory.read_word(pc);
    pc += 2;
    registers[rd] = memory.read_word(address);
    break;
  }

  case OP_STORE_IND:
    // Store to memory[Rd]
    memory.write_word(registers[rd], registers[rs]);
    break;

  case OP_STORE_DIR: {
    // Store to direct address (next word)
    word_t address = memory.read_word(pc);
    pc += 2;
    memory.write_word(address, registers[rs]);
    break;
  }

  // Arithmetic
  case OP_ADD:
    registers[rd] = ALU::add(registers[rs], registers[rt], flags);
    break;

  case OP_ADDI:
    registers[rd] = ALU::add(registers[rs], sign_extend_4bit(imm4), flags);
    break;

  case OP_SUB:
    registers[rd] = ALU::sub(registers[rs], registers[rt], flags);
    break;

  case OP_SUBI:
    registers[rd] = ALU::sub(registers[rs], sign_extend_4bit(imm4), flags);
    break;

  case OP_MUL:
    registers[rd] = ALU::mul(registers[rs], registers[rt], flags);
    break;

  case OP_DIV:
    registers[rd] = ALU::div(registers[rs], registers[rt], flags);
    break;

  case OP_INC:
    registers[rd] = ALU::add(registers[rd], 1, flags);
    break;

  case OP_DEC:
    registers[rd] = ALU::sub(registers[rd], 1, flags);
    break;

  // Logical
  case OP_AND:
    registers[rd] = ALU::and_op(registers[rs], registers[rt], flags);
    break;

  case OP_ANDI:
    registers[rd] = ALU::and_op(registers[rs], imm4, flags);
    break;

  case OP_OR:
    registers[rd] = ALU::or_op(registers[rs], registers[rt], flags);
    break;

  case OP_ORI:
    registers[rd] = ALU::or_op(registers[rs], imm4, flags);
    break;

  case OP_XOR:
    registers[rd] = ALU::xor_op(registers[rs], registers[rt], flags);
    break;

  case OP_NOT:
    registers[rd] = ALU::not_op(registers[rs], flags);
    break;

  // Shift
  case OP_SHL:
    registers[rd] = ALU::shl(registers[rs], registers[rt], flags);
    break;

  case OP_SHLI:
    registers[rd] = ALU::shl(registers[rs], imm4, flags);
    break;

  case OP_SHR:
    registers[rd] = ALU::shr(registers[rs], registers[rt], flags);
    break;

  case OP_SHRI:
    registers[rd] = ALU::shr(registers[rs], imm4, flags);
    break;

  // Comparison
  case OP_CMP:
    ALU::compare(registers[rs], registers[rt], flags);
    break;

  case OP_CMPI:
    ALU::compare(registers[rs], sign_extend_4bit(imm4), flags);
    break;

  // Branch/Jump
  case OP_JMP: {
    word_t address = memory.read_word(pc);
    pc = address;
    break;
  }

  case OP_JZ: {
    word_t address = memory.read_word(pc);
    pc += 2;
    if (flags & FLAG_ZERO) {
      pc = address;
    }
    break;
  }

  case OP_JNZ: {
    word_t address = memory.read_word(pc);
    pc += 2;
    if (!(flags & FLAG_ZERO)) {
      pc = address;
    }
    break;
  }

  case OP_JC: {
    word_t address = memory.read_word(pc);
    pc += 2;
    if (flags & FLAG_CARRY) {
      pc = address;
    }
    break;
  }

  case OP_JNC: {
    word_t address = memory.read_word(pc);
    pc += 2;
    if (!(flags & FLAG_CARRY)) {
      pc = address;
    }
    break;
  }

  case OP_JN: {
    word_t address = memory.read_word(pc);
    pc += 2;
    if (flags & FLAG_NEGATIVE) {
      pc = address;
    }
    break;
  }

  case OP_CALL: {
    word_t address = memory.read_word(pc);
    pc += 2;
    push(pc); // Save return address
    pc = address;
    break;
  }

  case OP_RET:
    pc = pop(); // Restore return address
    break;

  // Stack
  case OP_PUSH:
    push(registers[rs]);
    break;

  case OP_POP:
    registers[rd] = pop();
    break;

  // System
  case OP_HALT:
    halt();
    if (debug_mode) {
      std::cout << "CPU HALTED" << std::endl;
    }
    break;

  default:
    std::cerr << "Unknown opcode: 0x" << std::hex << (int)opcode << std::dec
              << std::endl;
    halt();
    break;
  }
}

void CPU::print_registers() const {
  std::cout << "Registers: ";
  for (int i = 0; i < NUM_REGISTERS; i++) {
    std::cout << "R" << i << "=0x" << std::hex << std::setw(4)
              << std::setfill('0') << registers[i] << " ";
  }
  std::cout << "PC=0x" << std::setw(4) << std::setfill('0') << pc << " SP=0x"
            << std::setw(4) << std::setfill('0') << sp << std::dec << std::endl;
}

void CPU::print_flags() const {
  std::cout << "Flags: ";
  std::cout << "Z=" << ((flags & FLAG_ZERO) ? 1 : 0) << " ";
  std::cout << "C=" << ((flags & FLAG_CARRY) ? 1 : 0) << " ";
  std::cout << "N=" << ((flags & FLAG_NEGATIVE) ? 1 : 0) << " ";
  std::cout << "O=" << ((flags & FLAG_OVERFLOW) ? 1 : 0) << std::endl;
}

void CPU::disassemble_instruction(word_t instruction, addr_t address) const {
  byte_t opcode = GET_OPCODE(instruction);
  byte_t rd = GET_RD(instruction);
  byte_t rs = GET_RS(instruction);
  byte_t rt = GET_RT(instruction);
  byte_t imm4 = GET_IMM4(instruction);
  byte_t imm7 = GET_IMM7(instruction);

  std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << address
            << ": " << std::setw(4) << std::setfill('0') << instruction << "  "
            << get_opcode_name(opcode) << " ";

  // Format operands based on instruction type
  switch (opcode) {
  case OP_NOP:
    if (rd != rs) {
      std::cout << "R" << (int)rd << ", R" << (int)rs;
    }
    break;
  case OP_MOVI:
    std::cout << "R" << (int)rd << ", " << std::dec << sign_extend_7bit(imm7);
    break;
  case OP_LOAD_IND:
    std::cout << "R" << (int)rd << ", [R" << (int)rs << "]";
    break;
  case OP_STORE_IND:
    std::cout << "R" << (int)rs << ", [R" << (int)rd << "]";
    break;
  case OP_LOAD_DIR:
  case OP_STORE_DIR:
  case OP_JMP:
  case OP_JZ:
  case OP_JNZ:
  case OP_JC:
  case OP_JNC:
  case OP_JN:
  case OP_CALL:
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0')
              << memory.read_word(pc);
    break;
  case OP_ADDI:
  case OP_SUBI:
  case OP_ANDI:
  case OP_ORI:
  case OP_SHLI:
  case OP_SHRI:
    std::cout << "R" << (int)rd << ", R" << (int)rs << ", " << std::dec
              << sign_extend_4bit(imm4);
    break;
  case OP_CMPI:
    std::cout << "R" << (int)rs << ", " << std::dec << sign_extend_4bit(imm4);
    break;
  case OP_INC:
  case OP_DEC:
  case OP_PUSH:
  case OP_POP:
    std::cout << "R" << (int)rd;
    break;
  case OP_NOT:
  case OP_CMP:
    std::cout << "R" << (int)rd << ", R" << (int)rs;
    break;
  case OP_RET:
  case OP_HALT:
    // No operands
    break;
  default:
    // Three-operand format
    std::cout << "R" << (int)rd << ", R" << (int)rs << ", R" << (int)rt;
    break;
  }
  std::cout << std::dec;
}
