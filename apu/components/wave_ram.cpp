#include "wave_ram.h"
#include "audio_registers.h"

WaveRAM::WaveRAM(AudioRegisters& audio_registers) : audio_registers_(audio_registers) {}

void WaveRAM::set_frequency_low(uint8_t value) {
  timer_.set_frequency_low(value);
}

void WaveRAM::set_frequency_high(uint8_t value) {
  timer_.set_frequency_high(value);
}

uint8_t WaveRAM::output() const {
  if (volume_ == 0) {
    return 0;
  }
  return current_sample_;
}

void WaveRAM::set_volume(uint8_t volume) {
  volume_ = volume;
  volume_shift_ = volume_map_[volume];
}

void WaveRAM::master_enable() {
  ram_position_ = 1;
  timer_.reset_counter();
}

void WaveRAM::disable() {
  enabled_ = false;
  current_sample_ = 0;
}

void WaveRAM::load_sample(uint32_t apu_clock) {
  current_sample_ = audio_registers_.get_WAVE_RAM_internal(ram_position_ / 2, apu_clock);
  if (ram_position_ % 2 == 0) {
    current_sample_ = current_sample_ >> 4;
  } else {
    current_sample_ &= 0x0F;
  }
  current_sample_ >>= volume_shift_;
}

void WaveRAM::trigger() {
  ram_position_ = 0;
  enabled_ = true;
  timer_.reset_counter(6);
}

uint16_t WaveRAM::get_timer_counter() const {
  return timer_.get_counter();
}

uint8_t WaveRAM::ram_position() const {
  return ram_position_;
}