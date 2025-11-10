#pragma once

#include <cstdint>
#include "constants.h"
#include "memory_bridge.h"

class CPURegisters;
class MemoryController;

template <typename Bus>
class ProgramCounter {
public:
  ProgramCounter(CPURegisters& registers, FirstLevelMemoryBridge<Bus>& memory_bridge);

  // PC access
  uint16_t get() const;
  void set(uint16_t value);
  void set_high(uint16_t value);
  void set_low(uint16_t value);
  void increment(int16_t size);

  // Fetch instruction at PC (handles HALT_BUG logic)
  // Pass halt_state and clear_halt_bug callback to handle HALT_BUG
  template <typename ClearHaltBug>
  const unsigned char* fetch_instruction(HALT_STATE halt_state, ClearHaltBug& clear_halt_bug);

  // Read data at PC and increment
  uint8_t read_opcode_byte();  // Also sets current_opcode_
  uint8_t read_u8_at_pc();
  uint16_t read_u16_at_pc();

  // Current opcode access
  uint8_t current_opcode() const;

private:
  CPURegisters& registers_;
  FirstLevelMemoryBridge<Bus>& memory_bridge_;
  uint8_t current_opcode_ = 0;
};

#include "program_counter.inc"