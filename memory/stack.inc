#include "cpu_registers.h"
#include "memory_controller.h"
#include "stack.h"

template <typename Bus>
Stack<Bus>::Stack(FirstLevelMemoryBridge<Bus>& memory_bridge, CPURegisters& registers)
    : memory_bridge_(memory_bridge), registers_(registers) {}

template <typename Bus>
void Stack<Bus>::push_16(uint16_t value) {
  registers_.SP() = registers_.SP().get() - 2;
  memory_bridge_.write(registers_.SP().get() + 1, value >> 8);
  memory_bridge_.write(registers_.SP().get(), value & BYTE_MASK);
}

template <typename Bus>
void Stack<Bus>::push_8(uint8_t value) {
  registers_.SP() = registers_.SP().get() - 1;
  memory_bridge_.write(registers_.SP().get(), value);
}

template <typename Bus>
uint16_t Stack<Bus>::pop_16() {
  uint16_t value = (*memory_bridge_.read(registers_.SP().get() + 1) << 8);
  value |= (*memory_bridge_.read(registers_.SP().get()));
  registers_.SP() = registers_.SP().get() + 2;
  return value;
}

template <typename Bus>
uint8_t Stack<Bus>::pop_8() {
  uint8_t value = (*memory_bridge_.read(registers_.SP().get()));
  registers_.SP() = registers_.SP().get() + 1;
  return value;
}
