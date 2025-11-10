#include "dac.h"
#include "LFSR.h"
#include "audio_constants.h"
#include "frame_sequencer.h"
#include "wave_duty.h"

// DAC output normalization constant
constexpr float DAC_NORMALIZATION = 7.5f;

template <typename Frequency>
void DAC<Frequency>::enabled(bool enabled) {
  enabled_ = enabled ? CHANNEL_ENABLED : CHANNEL_DISABLED;
}

// Template implementations
template <typename Frequency>
DAC<Frequency>::DAC(AudioRegisters& audio_registers, FrameSequencer& frame_sequencer) : envelope_() {
  frame_sequencer.add_envelope(&envelope_);
}

template <typename Frequency>
void DAC<Frequency>::set_LFSR(uint8_t clock_shift, uint8_t width, uint8_t divider) {
  if constexpr (std::is_same_v<Frequency, LFSR>) {
    duty_.set_LFSR(clock_shift, width, divider);
  }
}

template <typename Frequency>
void DAC<Frequency>::set_duty(uint8_t duty) {
  if constexpr (std::is_same_v<Frequency, WaveDuty>) {
    duty_.set_duty(duty);
  }
}

template <typename Frequency>
void DAC<Frequency>::master_enable() {
  duty_.master_enable();
}

template <typename Frequency>
void DAC<Frequency>::set_envelope(uint8_t volume, bool envelope_direction, uint8_t sweep_pace) {
  next_envelope_.trigger(volume, envelope_direction, sweep_pace);
  enabled(volume > 0 || envelope_direction);
}

template <typename Frequency>
void DAC<Frequency>::trigger() {
  envelope_ = next_envelope_;
  duty_.trigger();
}

template <typename Frequency>
void DAC<Frequency>::set_frequency_low(uint8_t value) {
  if constexpr (std::is_same_v<Frequency, WaveDuty>) {
    duty_.set_frequency_low(value);
  }
}

template <typename Frequency>
void DAC<Frequency>::set_frequency_high(uint8_t value) {
  if constexpr (std::is_same_v<Frequency, WaveDuty>) {
    duty_.set_frequency_high(value);
  }
}

template <typename Frequency>
uint16_t DAC<Frequency>::get_frequency() const {
  if constexpr (std::is_same_v<Frequency, WaveDuty>) {
    return duty_.get_frequency();
  }
  return 0;
}

template <typename Frequency>
float DAC<Frequency>::output() const {
  auto input = duty_.get_output() * envelope_.output_volume();
  return (static_cast<float>(input) / DAC_NORMALIZATION) - 1.0f;
}

// Explicit template instantiation
template class DAC<WaveDuty>;
template class DAC<LFSR>;

WaveRAMDAC::WaveRAMDAC(AudioRegisters& audio_registers, FrameSequencer& frame_sequencer)
    : wave_ram_(audio_registers) {}

void WaveRAMDAC::set_volume(uint8_t volume) {
  wave_ram_.set_volume(volume);
}

float WaveRAMDAC::output() const {
  return (static_cast<float>(wave_ram_.output()) / DAC_NORMALIZATION) - 1.0f;
}

void WaveRAMDAC::set_frequency_low(uint8_t value) {
  wave_ram_.set_frequency_low(value);
}

void WaveRAMDAC::set_frequency_high(uint8_t value) {
  wave_ram_.set_frequency_high(value);
}

void WaveRAMDAC::master_enable() {
  wave_ram_.master_enable();
}

void WaveRAMDAC::enabled(bool enabled) {
  enabled_ = enabled ? CHANNEL_ENABLED : CHANNEL_DISABLED;
  if (!enabled) {
    wave_ram_.disable();
  }
}

void WaveRAMDAC::disable_wave_ram() {
  wave_ram_.disable();
}

void WaveRAMDAC::trigger() {
  wave_ram_.trigger();
}

uint8_t WaveRAMDAC::ram_position() const {
  return wave_ram_.ram_position();
}

uint16_t WaveRAMDAC::get_timer_counter() const {
  return wave_ram_.get_timer_counter();
}