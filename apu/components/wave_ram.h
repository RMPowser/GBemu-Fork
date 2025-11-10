#pragma once
#include <array>
#include <cstdint>
#include "frequency_timer.h"

class AudioRegisters;

class WaveRAM {
public:
  WaveRAM(AudioRegisters& audio_registers);
  void master_enable();
  void set_frequency_low(uint8_t value);
  void set_frequency_high(uint8_t value);

  [[gnu::always_inline]] void tick(uint32_t apu_clock) {
    if (!enabled_) {
      return;
    }
    int8_t triggered = timer_.tick(apu_clock);

    if (triggered >= 0) {
      ram_position_ = (ram_position_ + 1) % 32;
      load_sample(apu_clock - (4 - (triggered + 1)));
    }
  }

  void set_volume(uint8_t volume);
  uint8_t output() const;
  void trigger();
  uint8_t ram_position() const;

  uint16_t get_timer_counter() const;

  void disable();

private:
  void load_sample(uint32_t apu_clock);
  AudioRegisters& audio_registers_;
  uint8_t current_sample_ = 0;
  uint8_t ram_position_ = 1;
  FrequencyTimerChannel3 timer_;

  uint8_t volume_ = 0;
  std::array<uint8_t, 4> volume_map_ = {4, 0, 1, 2};
  uint8_t volume_shift_ = 0;

  bool enabled_ = false;
};