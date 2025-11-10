#include "wave_duty.h"
#include <cstdint>

WaveDuty::WaveDuty() = default;

void WaveDuty::set_frequency_low(uint8_t value) {
  timer_.set_frequency_low(value);
}

void WaveDuty::set_frequency_high(uint8_t value) {
  timer_.set_frequency_high(value);
}

void WaveDuty::set_duty(uint8_t duty) {
  duty_pattern_ = duty;
}

void WaveDuty::master_enable() {
  duty_position_ = 0;
}

uint8_t WaveDuty::get_output() const {
  return duty_patterns[duty_pattern_][duty_position_];
}

uint16_t WaveDuty::get_frequency() const {
  return timer_.get_frequency();
}