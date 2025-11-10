#pragma once

#include <cstdint>

template <uint16_t frequencyShift>
class FrequencyTimerInternal {
public:
  FrequencyTimerInternal() { reset_counter(); }

  void set_frequency_low(uint8_t value);
  void set_frequency_high(uint8_t value);
  uint16_t get_frequency() const;

  void reset_counter(uint16_t extra_offset = 0);
  uint16_t get_counter() const;

  [[gnu::always_inline]] int8_t tick(uint32_t apu_clock) {
    int8_t triggered = (counter_ <= 4) ? (counter_ - 1) : -1;
    counter_ = (counter_ <= 4) ? (counter_refresh_ - (4 - counter_)) : (counter_ - 4);
    return triggered;
  }

private:
  uint16_t frequency_ = 0;
  uint16_t counter_ = 0;
  uint16_t counter_refresh_ = 0;
};

using FrequencyTimer = FrequencyTimerInternal<0>;
using FrequencyTimerChannel3 = FrequencyTimerInternal<1>;

class FrequencyTimerChannel4 {
public:
  FrequencyTimerChannel4();
  void set_frequency(uint8_t shift, uint8_t divisor);
  [[gnu::always_inline]] bool tick(uint32_t apu_clock) {
    const bool triggered = counter_ <= 4;
    counter_ = triggered ? (counter_refresh_ - (4 - counter_)) : (counter_ - 4);
    return triggered;
  }
  void reset_counter();

private:
  uint8_t shift_ = 0;
  uint8_t divisor_ = 0;
  uint16_t counter_ = 0;
  uint16_t counter_refresh_ = 0;
};