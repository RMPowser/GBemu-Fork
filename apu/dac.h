#pragma once

#include "audio_constants.h"
#include "envelope.h"
#include "wave_ram.h"

class FrameSequencer;
class AudioRegisters;

template <typename Frequency>
class DAC {
public:
  DAC(AudioRegisters& audio_registers, FrameSequencer& frame_sequencer);
  [[gnu::always_inline]] void tick(uint32_t apu_clock) { duty_.tick(apu_clock); }

  float output() const;

  void enabled(bool enabled);
  [[gnu::always_inline]] float enabled() const { return enabled_; }

  //Enables
  void trigger();
  void master_enable();

  //LFSR only for Noise
  void set_LFSR(uint8_t clock_shift, uint8_t width, uint8_t divider);

  //Duty - only for Square Wave
  void set_duty(uint8_t duty);

  //Envelope
  void set_envelope(uint8_t volume, bool envelope_direction, uint8_t sweep_pace);

  //Frequency Forwarding
  uint16_t get_frequency() const;
  void set_frequency_low(uint8_t value);
  void set_frequency_high(uint8_t value);

private:
  float enabled_ = CHANNEL_DISABLED;
  Frequency duty_;

  Envelope envelope_;
  Envelope next_envelope_;
};

class WaveRAMDAC {
public:
  WaveRAMDAC(AudioRegisters& audio_registers, FrameSequencer& frame_sequencer);
  float output() const;
  void set_frequency_low(uint8_t value);
  void set_frequency_high(uint8_t value);

  void enabled(bool enabled);
  [[gnu::always_inline]] float enabled() const { return enabled_; }
  [[gnu::always_inline]] void tick(uint32_t apu_clock) { wave_ram_.tick(apu_clock); }

  void set_volume(uint8_t volume);
  void master_enable();

  void trigger();
  uint8_t ram_position() const;
  void disable_wave_ram();
  uint16_t get_timer_counter() const;

private:
  float enabled_ = CHANNEL_DISABLED;
  WaveRAM wave_ram_;
};