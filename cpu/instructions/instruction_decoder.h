#pragma once
#include <inttypes.h>

template <typename Bus>
class CPU;

class InstructionDecoder {
public:
  template <typename Bus>
  static void decode_and_execute(uint8_t opcode, CPU<Bus>* cpu);

private:
  template <typename Bus>
  static void execute_cb(CPU<Bus>* cpu);
};

#include "instruction_decoder.inc"