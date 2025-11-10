#pragma once

#include <cstdint>
#include "memory_bridge.h"

class CPURegisters;

template <typename Bus>
class Stack {
public:
  Stack(FirstLevelMemoryBridge<Bus>& memory_bridge, CPURegisters& registers);

  void push_16(uint16_t value);
  void push_8(uint8_t value);
  uint16_t pop_16();
  uint8_t pop_8();

private:
  FirstLevelMemoryBridge<Bus>& memory_bridge_;
  CPURegisters& registers_;
};

#include "stack.inc"