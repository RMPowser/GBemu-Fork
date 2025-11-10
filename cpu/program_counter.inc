#include <iostream>
#include "constants.h"
#include "cpu_registers.h"
#include "interrupt_controller.h"
#include "memory_bridge.h"
#include "memory_controller.h"
#include "program_counter.h"

template <typename Bus>
ProgramCounter<Bus>::ProgramCounter(CPURegisters& registers, FirstLevelMemoryBridge<Bus>& memory_bridge)
    : registers_(registers), memory_bridge_(memory_bridge) {}

template <typename Bus>
uint16_t ProgramCounter<Bus>::get() const {
  return registers_.pc().get();
}

template <typename Bus>
void ProgramCounter<Bus>::set(uint16_t value) {
  registers_.pc() = value;
}

template <typename Bus>
void ProgramCounter<Bus>::set_high(uint16_t value) {
  registers_.pc() = value << 8 | (registers_.pc().get() & BYTE_MASK);
}

template <typename Bus>
void ProgramCounter<Bus>::set_low(uint16_t value) {
  registers_.pc() = (registers_.pc().get() & 0xFF00) | value;
}

template <typename Bus>
void ProgramCounter<Bus>::increment(int16_t size) {
  registers_.pc() = registers_.pc().get() + size;
}

template <typename Bus>
template <typename ClearHaltBug>
const unsigned char* ProgramCounter<Bus>::fetch_instruction(HALT_STATE halt_state,
                                                            ClearHaltBug& clear_halt_bug) {
  const uint16_t address = registers_.pc().get();
  VERBOSE_PRINT() << std::endl << "Fetching instruction at: " << address << std::endl;

  if (halt_state == NO_HALT) {
    increment(1);
  } else {  //Otherwise we're in HALT_BUG
    clear_halt_bug();
  }

  auto data = memory_bridge_.read(address);
  current_opcode_ = *data;
  return data;
}

template <typename Bus>
uint8_t ProgramCounter<Bus>::read_opcode_byte() {
  uint8_t value = *memory_bridge_.read(registers_.pc().get());
  current_opcode_ = value;
  increment(1);
  return value;
}

template <typename Bus>
uint8_t ProgramCounter<Bus>::read_u8_at_pc() {
  uint8_t value = *memory_bridge_.read(registers_.pc().get());
  increment(1);
  return value;
}

template <typename Bus>
uint16_t ProgramCounter<Bus>::read_u16_at_pc() {
  uint16_t value = *reinterpret_cast<const uint16_t*>(memory_bridge_.read(registers_.pc().get()));
  increment(2);
  return value;
}

template <typename Bus>
uint8_t ProgramCounter<Bus>::current_opcode() const {
  return current_opcode_;
}
