#include "joypad.h"
#include <cstdint>
#include "hardware_registers.h"

namespace {
constexpr uint8_t SSBA_MASK = 0x20;
constexpr uint8_t JOYPAD_MASK = 0x10;
constexpr uint8_t BUTTON_A_BIT = 0x01;
constexpr uint8_t BUTTON_B_BIT = 0x02;
constexpr uint8_t BUTTON_SELECT_BIT = 0x04;
constexpr uint8_t BUTTON_START_BIT = 0x08;
constexpr uint8_t ALL_BUTTONS_MASK = 0x0F;
}  // namespace

Joypad::Joypad(HardwareRegisters& hw_registers) : hw_registers_(hw_registers) {}

namespace {
void update_interrupts(bool& interrupt, bool& previous_value, bool& current_value) {
  if (!previous_value && current_value) {
    interrupt = true;
  } else if (!current_value) {
    interrupt = false;
  }
}
}  // namespace

void Joypad::update_state(JoypadState& joypad_state) {
  update_interrupts(joypad_interrupts_.a_pressed, joypad_state_.a_pressed, joypad_state.a_pressed);
  update_interrupts(joypad_interrupts_.b_pressed, joypad_state_.b_pressed, joypad_state.b_pressed);
  update_interrupts(joypad_interrupts_.select_pressed, joypad_state_.select_pressed,
                    joypad_state.select_pressed);
  update_interrupts(joypad_interrupts_.start_pressed, joypad_state_.start_pressed,
                    joypad_state.start_pressed);
  update_interrupts(joypad_interrupts_.up_pressed, joypad_state_.up_pressed, joypad_state.up_pressed);
  update_interrupts(joypad_interrupts_.down_pressed, joypad_state_.down_pressed, joypad_state.down_pressed);
  update_interrupts(joypad_interrupts_.left_pressed, joypad_state_.left_pressed, joypad_state.left_pressed);
  update_interrupts(joypad_interrupts_.right_pressed, joypad_state_.right_pressed,
                    joypad_state.right_pressed);

  joypad_state_ = joypad_state;
  joypad_write();
}

void Joypad::handle_button(bool pressed, uint8_t bit) {
  if (!pressed) {
    hw_registers_.set_P1_JOYP_internal(hw_registers_.get_P1_JOYP() | bit);
  } else {
    hw_registers_.set_P1_JOYP_internal(hw_registers_.get_P1_JOYP() & ~bit);
  }
}

void Joypad::joypad_write() {
  uint8_t previous_value = hw_registers_.get_P1_JOYP();

  bool trigger_interrupt = false;
  if ((previous_value & SSBA_MASK) == 0) {
    handle_button(joypad_state_.a_pressed, BUTTON_A_BIT);
    handle_button(joypad_state_.b_pressed, BUTTON_B_BIT);
    handle_button(joypad_state_.select_pressed, BUTTON_SELECT_BIT);
    handle_button(joypad_state_.start_pressed, BUTTON_START_BIT);

    trigger_interrupt = trigger_interrupt || joypad_interrupts_.a_pressed || joypad_interrupts_.b_pressed ||
                        joypad_interrupts_.select_pressed || joypad_interrupts_.start_pressed;

    joypad_interrupts_.a_pressed = false;
    joypad_interrupts_.b_pressed = false;
    joypad_interrupts_.select_pressed = false;
    joypad_interrupts_.start_pressed = false;

  } else if ((previous_value & JOYPAD_MASK) == 0) {
    handle_button(joypad_state_.up_pressed, BUTTON_SELECT_BIT);
    handle_button(joypad_state_.down_pressed, BUTTON_START_BIT);
    handle_button(joypad_state_.left_pressed, BUTTON_B_BIT);
    handle_button(joypad_state_.right_pressed, BUTTON_A_BIT);

    trigger_interrupt = trigger_interrupt || joypad_interrupts_.up_pressed ||
                        joypad_interrupts_.down_pressed || joypad_interrupts_.left_pressed ||
                        joypad_interrupts_.right_pressed;

    joypad_interrupts_.up_pressed = false;
    joypad_interrupts_.down_pressed = false;
    joypad_interrupts_.left_pressed = false;
    joypad_interrupts_.right_pressed = false;
  } else {
    hw_registers_.set_P1_JOYP_internal(previous_value | ALL_BUTTONS_MASK);
  }

  if (trigger_interrupt) {
    hw_registers_.trigger_joypad_interrupt();
  }
}
