#include "noise_channel.h"
#include "audio_constants.h"
#include "audio_registers.h"
#include "frame_sequencer.h"

NoiseChannel::NoiseChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers)
    : ChannelBase<NoiseChannel, Channel4Traits>(frame_sequencer, audio_registers) {}

void NoiseChannel::audio_register_write(uint16_t address, uint8_t value) {
  switch (address) {
    case NR25_ADDR: {
      bool left_enabled = (value & 0x08) != 0;
      bool right_enabled = (value & 0x80) != 0;
      left_enabled_ = static_cast<float>(left_enabled) * PANNING_MULTIPLIER;
      right_enabled_ = static_cast<float>(right_enabled) * PANNING_MULTIPLIER;
    } break;
    case NR20_ADDR:
      length_timer_.set_length(value & LENGTH_COUNTER_MASK);
      break;
    case NR21_ADDR: {
      uint8_t volume = value >> 4;
      uint8_t envelope_direction = (value >> 3) & 0x01;
      uint8_t sweep_pace = value & 0x07;
      dac_.set_envelope(volume, envelope_direction, sweep_pace);
    } break;
    case NR22_ADDR: {
      uint8_t clock_shift = value >> 4;
      uint8_t width = (value >> 3) & 1;
      uint8_t divider = value & 0x07;
      dac_.set_LFSR(clock_shift, width, divider);
      if (clock_shift > MAX_NOISE_CLOCK_SHIFT) {
        disable_channel();
      }
    } break;
    case NR23_ADDR:
      length_timer_.enable((value >> 6) & 1);
      bool trigger = (value >> 7) & 1;
      if (trigger) {
        enable_channel();
        length_timer_.trigger();
        dac_.trigger();
      }
  }
}
