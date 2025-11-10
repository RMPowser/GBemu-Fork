#pragma once

#include <cstdint>
#include "frequency_timer.h"

class LFSR {
public:
  LFSR();
  [[gnu::always_inline]] void tick(uint32_t apu_clock) {
    if (timer_.tick(apu_clock)) {
      bool feedback = ((lfsr_ & 1) ^ (lfsr_ >> 1)) & 1;
      lfsr_ = (lfsr_ >> 1) | (feedback << 14);
      if (width_) {
        lfsr_ &= ~(1 << 6);
        lfsr_ |= (feedback << 6);
      }
    }
  }
  uint8_t get_output() const;

  void set_LFSR(uint8_t clock_shift, uint8_t width, uint8_t divider);

  void trigger();
  void master_enable();

private:
  FrequencyTimerChannel4 timer_;
  uint16_t lfsr_ = 0xFFFF;
  bool width_ = false;
};
