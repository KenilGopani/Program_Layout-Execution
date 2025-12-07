/**
 * Memory Implementation
 * 
 * This file implements the memory subsystem for our CPU emulator.
 */

#include "memory.h"
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

Memory::Memory() { clear(); }

/**
 * Clear all memory to zero
 */
void Memory::clear() { memset(data, 0, MEMORY_SIZE); }

/**
 * Read a single byte from memory
 */
byte_t Memory::read_byte(addr_t address) const { return data[address]; }

/**
 * Write a single byte to memory
 * Handles memory-mapped I/O for console output at address 0xF000
 */
void Memory::write_byte(addr_t address, byte_t value) {
  // Check for memory-mapped I/O write
  if (address == IO_CONSOLE_OUT) {
    // Write character to console immediately
    std::cout << (char)value << std::flush;
    return;
  }

  // Normal memory write
  data[address] = value;
}

/**
 * Read a 16-bit word from memory
 * Uses little-endian format: low byte at lower address
 */
word_t Memory::read_word(addr_t address) const {
  byte_t low = read_byte(address);
  byte_t high = read_byte(address + 1);
  return (word_t)((high << 8) | low);
}

/**
 * Write a 16-bit word to memory
 * Uses little-endian format: low byte at lower address
 */
void Memory::write_word(addr_t address, word_t value) {
  write_byte(address, (byte_t)(value & 0xFF));          // Low byte
  write_byte(address + 1, (byte_t)((value >> 8) & 0xFF)); // High byte
}

/**
 * Load a binary program file into memory
 * Returns true on success, false on error
 */
bool Memory::load_program(const std::string &filename, addr_t start_address) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
    return false;
  }

  // Determine file size
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Verify program fits in available memory
  if (start_address + size > MEMORY_SIZE) {
    std::cerr << "Error: Program too large for memory" << std::endl;
    return false;
  }

  // Read entire file into memory at specified address
  if (!file.read((char *)(data + start_address), size)) {
    std::cerr << "Error: Failed to read file" << std::endl;
    return false;
  }

  std::cout << "Loaded " << size << " bytes from '" << filename
            << "' at address 0x" << std::hex << std::setw(4)
            << std::setfill('0') << start_address << std::dec << std::endl;

  return true;
}

/**
 * Dump memory contents in hexadecimal and ASCII format
 * Useful for debugging and inspecting memory state
 */
void Memory::dump(addr_t start, addr_t end) const {
  std::cout << "\nMemory Dump [0x" << std::hex << std::setw(4)
            << std::setfill('0') << start << " - 0x" << std::setw(4)
            << std::setfill('0') << end << "]:\n";

  for (addr_t addr = start; addr <= end; addr += 16) {
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << addr
              << ": ";

    // Display 16 bytes in hexadecimal
    for (int i = 0; i < 16 && (addr + i) <= end; i++) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << (int)data[addr + i] << " ";
    }

    // Display ASCII representation for printable characters
    std::cout << " | ";
    for (int i = 0; i < 16 && (addr + i) <= end; i++) {
      byte_t b = data[addr + i];
      if (b >= 32 && b < 127) {
        std::cout << (char)b;
      } else {
        std::cout << ".";
      }
    }

    std::cout << std::endl;
  }
  std::cout << std::dec << std::endl;
}

void Memory::dump_range(addr_t start, size_t length) const {
  if (length == 0)
    return;
  addr_t end = start + length - 1;
  if (end < start)
    end = 0xFFFF; // Handle overflow
  dump(start, end);
}
