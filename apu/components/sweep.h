#pragma once

#include <cstdint>
#include <functional>

class AudioRegisters;

class Sweep {
public:
  Sweep(AudioRegisters& audio_registers);
  void configure(uint8_t pace, bool negate, uint8_t shift);
  void tick();
  void trigger(uint16_t initial_frequency);
  void set_overflow_callback(std::function<void()> callback);
  void set_frequency_update_callback(std::function<void(uint16_t)> callback);

private:
  uint16_t calculate_new_frequency();
  void write_frequency_to_registers(uint16_t frequency);
  bool check_overflow(uint16_t frequency);

  AudioRegisters& audio_registers_;
  std::function<void()> overflow_callback_;
  std::function<void(uint16_t)> frequency_update_callback_;
  uint16_t shadow_frequency_ = 0;
  uint32_t calculation_count_ = 0;
  uint8_t pace_ = 0;
  uint8_t timer_ = 0;
  bool negate_ = false;
  uint8_t shift_ = 0;
  bool enabled_ = false;
};
