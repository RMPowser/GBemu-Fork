#include "LFSR.h"

LFSR::LFSR() = default;

void LFSR::set_LFSR(uint8_t clock_shift, uint8_t width, uint8_t divider) {
  width_ = width;
  timer_.set_frequency(clock_shift, divider);
}

void LFSR::master_enable() {}

void LFSR::trigger() {
  lfsr_ = 0xFFFF;
  timer_.reset_counter();
}

uint8_t LFSR::get_output() const {
  return ~lfsr_ & 1;
}
