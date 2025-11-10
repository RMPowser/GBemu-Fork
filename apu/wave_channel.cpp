#include "wave_channel.h"
#include "audio_constants.h"
#include "audio_registers.h"
#include "frame_sequencer.h"

WaveChannel::WaveChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers)
    : ChannelBase<WaveChannel, Channel3Traits>(frame_sequencer, audio_registers) {}

void WaveChannel::audio_register_write(uint16_t address, uint8_t value) {
  switch (address) {
    case NR1A_ADDR: {
      const bool enabled = (value >> 7) & 1;
      dac_.enabled(enabled);
      if (!enabled) {
        disable_channel();
      }
    } break;
    case NR1B_ADDR:
      length_timer_.set_length(value);
      break;
    case NR1C_ADDR: {
      dac_.set_volume((value >> 5) & 0x03);
      break;
    }
    case NR1D_ADDR:
      dac_.set_frequency_low(value);
      break;
    case NR1E_ADDR: {
      dac_.set_frequency_high(value & 0x07);
      length_timer_.enable((value >> 6) & 1);
      bool trigger = (value >> 7) & 1;
      if (trigger) {
        if (enabled_ && dac_.get_timer_counter() == 2) {
          corrupt_wave_ram();
        }
        enable_channel();
        length_timer_.trigger();
        dac_.trigger();
      }
    } break;
    case NR25_ADDR: {
      const bool left_enabled = (value >> 6) & 1;
      const bool right_enabled = (value >> 2) & 1;
      left_enabled_ = static_cast<float>(left_enabled) * PANNING_MULTIPLIER;
      right_enabled_ = static_cast<float>(right_enabled) * PANNING_MULTIPLIER;
    } break;
  }
}

uint8_t WaveChannel::ram_position() const {
  return dac_.ram_position();
}

void WaveChannel::corrupt_wave_ram() {
  const uint8_t ram_position = (dac_.ram_position() + 1) % 32;
  const uint8_t value = audio_registers_.get_WAVE_RAM(ram_position >> 1);
  switch (ram_position >> 3) {
    case 0:
      audio_registers_.set_WAVE_RAM(0, value);
      break;
    case 1:
    case 2:
    case 3: {
      uint8_t start_position = (ram_position >> 1) & 12;
      for (uint8_t i = 0; i < 4; i++) {
        audio_registers_.set_WAVE_RAM(i, audio_registers_.get_WAVE_RAM(start_position + i));
      }

    } break;
  }
}