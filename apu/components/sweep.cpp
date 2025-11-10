#include "sweep.h"
#include "audio_registers.h"

// Sweep frequency overflow threshold
constexpr uint16_t MAX_FREQUENCY = 2047;

// Sweep timer reload value when pace is 0
constexpr uint8_t SWEEP_TIMER_RELOAD = 8;

Sweep::Sweep(AudioRegisters& audio_registers) : audio_registers_(audio_registers) {}

void Sweep::configure(uint8_t pace, bool negate, uint8_t shift) {
  if (negate_ && !negate && calculation_count_ != 0) {
    overflow_callback_();
  }

  calculation_count_ = 0;
  pace_ = pace;
  negate_ = negate;
  shift_ = shift;
}

void Sweep::set_overflow_callback(std::function<void()> callback) {
  overflow_callback_ = callback;
}

void Sweep::set_frequency_update_callback(std::function<void(uint16_t)> callback) {
  frequency_update_callback_ = callback;
}

void Sweep::tick() {
  if (!enabled_) {
    return;
  }

  if (timer_ > 0)
    timer_--;

  if (timer_ != 0) {
    return;
  }

  timer_ = pace_ ? pace_ : SWEEP_TIMER_RELOAD;

  if (enabled_ && pace_ > 0) {
    const uint16_t new_frequency = calculate_new_frequency();
    if (check_overflow(new_frequency)) {
      return;
    }

    if (shift_ > 0) {
      shadow_frequency_ = new_frequency;
      write_frequency_to_registers(new_frequency);
      if (check_overflow(calculate_new_frequency())) {
        return;
      }
    }
  }
}

void Sweep::trigger(uint16_t initial_frequency) {
  shadow_frequency_ = initial_frequency;
  timer_ = pace_ ? pace_ : SWEEP_TIMER_RELOAD;
  enabled_ = (pace_ > 0 || shift_ > 0);

  if (enabled_ && shift_ > 0) {
    check_overflow(calculate_new_frequency());
  }
}

uint16_t Sweep::calculate_new_frequency() {
  calculation_count_++;
  const uint16_t delta = shadow_frequency_ >> shift_;

  if (negate_) {
    return shadow_frequency_ - delta;
  } else {
    return shadow_frequency_ + delta;
  }
}

bool Sweep::check_overflow(uint16_t frequency) {
  if (negate_) {
    return false;
  }

  bool overflow = frequency > MAX_FREQUENCY;
  if (!overflow)
    return false;

  enabled_ = false;
  overflow_callback_();

  return true;
}

void Sweep::write_frequency_to_registers(uint16_t frequency) {
  // Write directly to audio registers
  audio_registers_.set_NR13(frequency & 0xFF);
  audio_registers_.set_NR14((audio_registers_.get_NR14() & 0xF8) | ((frequency >> 8) & 0x07));

  // Call the frequency update callback to update the DAC directly
  if (frequency_update_callback_) {
    frequency_update_callback_(frequency);
  }
}
