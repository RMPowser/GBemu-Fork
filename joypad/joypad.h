#pragma once

#include <inttypes.h>
#include "joypad_state.h"

class HardwareRegisters;

class Joypad {
public:
  Joypad(HardwareRegisters& hw_registers);
  void update_state(JoypadState& joypad_state);
  void joypad_write();

private:
  void handle_button(bool pressed, uint8_t bit);

  HardwareRegisters& hw_registers_;
  JoypadState joypad_state_;
  JoypadState joypad_interrupts_;
};