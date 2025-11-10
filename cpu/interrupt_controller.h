#pragma once

#include <cstdint>
#include <iostream>
#include "constants.h"
#include "hardware_registers.h"
#include "program_counter.h"

class CPURegisters;
class SaveStateSerializer;

template <typename Bus>
class InterruptController {
public:
  InterruptController(HardwareRegisters& hw_registers, ProgramCounter<Bus>& pc);

  void enable_interrupts();
  void disable_interrupts();
  bool is_pending() const;
  bool is_enabled() const;

  // Called after each instruction to handle delayed interrupt enable
  void check_for_enable();

  // Services interrupts if enabled and pending
  // Returns true if an interrupt was serviced
  template <typename PushPC>
  bool service_interrupts(PushPC push_pc) {
    if (!is_pending()) {
      return false;
    }

    // Wake up from HALT if interrupt is pending
    if (halt_state_ == HALT) {
      halt_state_ = NO_HALT;
    }

    if (!interrupt_master_enabled_) {
      return false;
    }

    disable_interrupts();
    const uint16_t pc = pc_.get();
    push_pc(pc >> 8);

    auto interrupt_flag = hw_registers_.get_IF();
    auto interrupt_enable = hw_registers_.get_IE();

    push_pc(pc & BYTE_MASK);

    if (interrupt_enable & interrupt_flag & VBLANK_INTERRUPT_FLAG) {
      VERBOSE_PRINT() << "VBlank interrupt" << std::endl;
      hw_registers_.set_IF(interrupt_flag ^ VBLANK_INTERRUPT_FLAG);
      pc_.set(VBLANK_VECTOR);
    } else if (interrupt_enable & interrupt_flag & LCD_STAT_INTERRUPT_FLAG) {
      VERBOSE_PRINT() << "LCD/STAT interrupt" << std::endl;
      hw_registers_.set_IF(interrupt_flag ^ LCD_STAT_INTERRUPT_FLAG);
      pc_.set(LCD_STAT_VECTOR);
    } else if (interrupt_enable & interrupt_flag & TIMER_INTERRUPT_FLAG) {
      VERBOSE_PRINT() << "Timer interrupt" << std::endl;
      hw_registers_.set_IF(interrupt_flag ^ TIMER_INTERRUPT_FLAG);
      pc_.set(TIMER_VECTOR);
    } else if (interrupt_enable & interrupt_flag & SERIAL_INTERRUPT_FLAG) {
      VERBOSE_PRINT() << "Serial interrupt" << std::endl;
      hw_registers_.set_IF(interrupt_flag ^ SERIAL_INTERRUPT_FLAG);
      pc_.set(SERIAL_VECTOR);
    } else if (interrupt_enable & interrupt_flag & JOYPAD_INTERRUPT_FLAG) {
      VERBOSE_PRINT() << "Joypad interrupt" << std::endl;
      hw_registers_.set_IF(interrupt_flag ^ JOYPAD_INTERRUPT_FLAG);
      pc_.set(JOYPAD_VECTOR);
    } else {
      pc_.set(0x00);
    }

    return true;
  }

  // Halt state management
  HALT_STATE halt_state() const;
  void halt();
  void stop();
  bool should_execute_instruction() const;
  void clear_halt_bug();

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  HardwareRegisters& hw_registers_;
  ProgramCounter<Bus>& pc_;

  int8_t interrupt_enable_called_ = 0;
  bool interrupt_master_enabled_ = false;
  HALT_STATE halt_state_ = NO_HALT;
};

#include "interrupt_controller.inc"