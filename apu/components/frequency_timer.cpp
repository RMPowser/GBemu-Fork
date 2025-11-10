#include "frequency_timer.h"

template <uint16_t frequencyShift>
void FrequencyTimerInternal<frequencyShift>::set_frequency_low(uint8_t value) {
  frequency_ = (frequency_ & 0xFF00) | value;
  counter_refresh_ = ((2048 - frequency_) * (4 >> frequencyShift));
}

template <uint16_t frequencyShift>
void FrequencyTimerInternal<frequencyShift>::set_frequency_high(uint8_t value) {
  frequency_ = (frequency_ & 0x00FF) | (value << 8);
  counter_refresh_ = ((2048 - frequency_) * (4 >> frequencyShift));
}

template <uint16_t frequencyShift>
uint16_t FrequencyTimerInternal<frequencyShift>::get_frequency() const {
  return frequency_;
}

template <uint16_t frequencyShift>
void FrequencyTimerInternal<frequencyShift>::reset_counter(uint16_t extra_offset) {
  counter_ = counter_refresh_ + extra_offset;
}

template <uint16_t frequencyShift>
uint16_t FrequencyTimerInternal<frequencyShift>::get_counter() const {
  return counter_;
}

FrequencyTimerChannel4::FrequencyTimerChannel4() {
  counter_ = (divisor_ > 0 ? (divisor_ << 4) : 8) << shift_;
}

void FrequencyTimerChannel4::set_frequency(uint8_t shift, uint8_t divisor) {
  shift_ = shift;
  divisor_ = divisor;
  counter_refresh_ = (divisor_ > 0 ? (divisor_ << 4) : 8) << shift_;
}

void FrequencyTimerChannel4::reset_counter() {
  counter_ = counter_refresh_;
}

// Explicit template instantiations
template class FrequencyTimerInternal<0>;
template class FrequencyTimerInternal<1>;