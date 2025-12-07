/**
 * Assembler Entry Point
 * 
 * This program converts assembly language source code (.asm files) into
 * binary machine code that can be executed by our CPU emulator.
 * 
 * The assembler performs a two-pass process:
 *   Pass 1: Collect all labels and calculate their addresses
 *   Pass 2: Generate machine code and resolve label references
 */

#include "assembler.h"
#include <iostream>

void print_usage(const char *program_name) {
  std::cout << "Usage: " << program_name << " <input.asm> <output.bin>\n";
  std::cout << "Assembles assembly code into binary machine code\n";
}

int main(int argc, char *argv[]) {
  // Verify we received exactly two arguments: input and output files
  if (argc != 3) {
    print_usage(argv[0]);
    return 1;
  }

  std::string input_file = argv[1];
  std::string output_file = argv[2];

  // Create assembler instance and process the file
  Assembler assembler;

  if (!assembler.assemble(input_file, output_file)) {
    return 1;  // Assembly failed - errors already printed
  }

  return 0;  
}
