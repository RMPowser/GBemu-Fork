#include "square_wave_channel.h"
#include "audio_constants.h"
#include "audio_registers.h"
#include "channel_traits.h"
#include "frame_sequencer.h"
#include "length_timer.h"

template <typename Traits>
SquareWaveChannel<Traits>::SquareWaveChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers)
    : ChannelBase<SquareWaveChannel<Traits>, Traits>(frame_sequencer, audio_registers),
      sweep_(audio_registers) {

  if constexpr (Traits::HAS_SWEEP) {
    frame_sequencer.add_sweep(&sweep_);
    // Set up callback for sweep overflow to disable channel
    sweep_.set_overflow_callback([this]() { disable_channel(); });
    // Set up callback for frequency updates to update DAC directly
    sweep_.set_frequency_update_callback([this](uint16_t frequency) {
      dac_.set_frequency_low(frequency & 0xFF);
      dac_.set_frequency_high((frequency >> 8) & 0x07);
    });
  }
}

template <typename Traits>
void SquareWaveChannel<Traits>::audio_register_write(uint16_t address, uint8_t value) {
  // Handle sweep register for Channel 1
  if constexpr (Traits::HAS_SWEEP) {
    if (address == Traits::SweepRegister) {
      uint8_t pace = (value >> 4) & 0x07;
      bool negate = (value >> 3) & 0x01;
      uint8_t shift = value & 0x07;
      sweep_.configure(pace, negate, shift);
      return;
    }
  }

  switch (address) {
    case Traits::LengthRegister:
      dac_.set_duty(value >> 6);
      length_timer_.set_length(value & LENGTH_COUNTER_MASK);
      break;
    case Traits::EnvelopeRegister: {
      uint8_t volume = value >> 4;
      uint8_t envelope_direction = (value >> 3) & 0x01;
      uint8_t sweep_pace = value & 0x07;
      dac_.set_envelope(volume, envelope_direction, sweep_pace);
      break;
    }
    case Traits::FrequencyLowRegister:
      dac_.set_frequency_low(value);
      break;
    case Traits::FrequencyHighRegister: {
      dac_.set_frequency_high(value & 0x07);
      length_timer_.enable((value >> 6) & 1);
      bool trigger = (value >> 7) & 1;
      if (trigger) {
        enable_channel();
        length_timer_.trigger();
        dac_.trigger();

        // Trigger sweep if this channel has it
        if constexpr (Traits::HAS_SWEEP) {
          uint16_t frequency = dac_.get_frequency();
          sweep_.trigger(frequency);
        }
      }
    } break;
    case NR25_ADDR: {
      bool left_enabled = (value >> Traits::PANNING_LEFT_BIT) & 1;
      bool right_enabled = (value >> Traits::PANNING_RIGHT_BIT) & 1;
      left_enabled_ = static_cast<float>(left_enabled) * PANNING_MULTIPLIER;
      right_enabled_ = static_cast<float>(right_enabled) * PANNING_MULTIPLIER;
    } break;
  }
}

// Explicit template instantiations
template class SquareWaveChannel<Channel1Traits>;
template class SquareWaveChannel<Channel2Traits>;
