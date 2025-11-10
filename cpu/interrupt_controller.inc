#include <iostream>
#include "constants.h"
#include "interrupt_controller.h"
#include "program_counter.h"
#include "save_state.h"

template <typename Bus>
InterruptController<Bus>::InterruptController(HardwareRegisters& hw_registers, ProgramCounter<Bus>& pc)
    : hw_registers_(hw_registers), pc_(pc) {}

// ===== Interrupt Controller Constants =====
namespace {
constexpr int8_t INTERRUPT_ENABLE_DELAY = 2;
}  // namespace

template <typename Bus>
void InterruptController<Bus>::enable_interrupts() {
  if (interrupt_enable_called_ > 0) {
    return;
  }
  interrupt_enable_called_ = INTERRUPT_ENABLE_DELAY;
}

template <typename Bus>
void InterruptController<Bus>::disable_interrupts() {
  interrupt_master_enabled_ = false;
  interrupt_enable_called_ = 0;
}

template <typename Bus>
bool InterruptController<Bus>::is_pending() const {
  auto interrupt_flag = hw_registers_.get_IF();
  auto interrupt_enable = hw_registers_.get_IE();
  return (interrupt_flag & interrupt_enable & ALL_INTERRUPTS_MASK);
}

template <typename Bus>
bool InterruptController<Bus>::is_enabled() const {
  return interrupt_master_enabled_;
}

template <typename Bus>
void InterruptController<Bus>::check_for_enable() {
  if (interrupt_enable_called_ > 0) {
    interrupt_enable_called_--;
    if (!interrupt_master_enabled_ && interrupt_enable_called_ == 0) {
      interrupt_master_enabled_ = true;
    }
  }
}

template <typename Bus>
HALT_STATE InterruptController<Bus>::halt_state() const {
  return halt_state_;
}

template <typename Bus>
void InterruptController<Bus>::halt() {
  if (interrupt_master_enabled_) {
    halt_state_ = HALT;
  } else {
    if (is_pending()) {
      halt_state_ = HALT_BUG;
      std::cout << "HALT_BUG" << std::endl;
    } else {
      halt_state_ = HALT;
    }
  }
}

template <typename Bus>
void InterruptController<Bus>::stop() {
  std::cout << "Stop" << std::endl;
  // TODO: Implement stop functionality
  // halt_state_ = HALT_AND_NO_EXECUTE;
}

template <typename Bus>
bool InterruptController<Bus>::should_execute_instruction() const {
  return halt_state_ == NO_HALT || halt_state_ == HALT_BUG;
}

template <typename Bus>
void InterruptController<Bus>::clear_halt_bug() {
  if (halt_state_ == HALT_BUG) {
    halt_state_ = NO_HALT;
  }
}

template <typename Bus>
void InterruptController<Bus>::serialize(SaveStateSerializer& serializer) const {
  serializer << interrupt_enable_called_;
  serializer << interrupt_master_enabled_;
  serializer << halt_state_;
}

template <typename Bus>
void InterruptController<Bus>::deserialize(SaveStateSerializer& serializer) {
  serializer >> interrupt_enable_called_;
  serializer >> interrupt_master_enabled_;
  serializer >> halt_state_;
}
