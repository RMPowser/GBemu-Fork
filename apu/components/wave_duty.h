#pragma once

#include <array>
#include <cstdint>
#include "frequency_timer.h"

class WaveDuty {
public:
  WaveDuty();

  [[gnu::always_inline]] void tick(uint32_t apu_clock) {
    int8_t triggered = timer_.tick(apu_clock);
    if (triggered >= 0) {
      duty_position_ = (duty_position_ + 1) % 8;
    }
  }

  void set_frequency_low(uint8_t value);
  void set_frequency_high(uint8_t value);
  void set_duty(uint8_t duty);

  uint8_t get_output() const;
  uint16_t get_frequency() const;

  void master_enable();
  void trigger() {}

private:
  uint8_t duty_position_ = 0;
  uint8_t duty_pattern_ = 0;
  FrequencyTimer timer_;
  constexpr static std::array<std::array<bool, 8>, 4> duty_patterns = {{{0, 0, 0, 0, 0, 0, 0, 1},
                                                                        {1, 0, 0, 0, 0, 0, 0, 1},
                                                                        {1, 0, 0, 0, 0, 1, 1, 1},
                                                                        {0, 1, 1, 1, 1, 1, 1, 0}}};
};
