#pragma once

#include <utility>
#include "audio_constants.h"
#include "audio_registers.h"
#include "dac.h"
#include "frame_sequencer.h"

template <typename Parent, typename Traits>
class ChannelBase {
public:
  ChannelBase(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers)
      : audio_registers_(audio_registers),
        length_timer_(frame_sequencer, [this]() { length_expired(); }),
        dac_(audio_registers, frame_sequencer) {
    frame_sequencer.add_length_timer(&length_timer_);
  }

  void length_expired() {
    disable_channel();
    if constexpr (std::is_same_v<typename Traits::DACType, WaveRAMDAC>) {
      dac_.disable_wave_ram();
    }
  }

  bool enabled() const { return enabled_; }

  void master_enable() { dac_.master_enable(); }

  [[gnu::always_inline]] std::pair<float, float> output() const {
    const float output = dac_.output() * enabled_ * length_timer_.should_play() * dac_.enabled();
    return {output * left_enabled_, output * right_enabled_};
  }

  [[gnu::always_inline]] void tick(uint32_t apu_clock) {
    dac_.tick(apu_clock);

    if (!enabled_)
      return;

    if (!dac_.enabled()) {
      disable_channel();
    }
  }

  void disable_channel() {
    enabled_ = CHANNEL_DISABLED;
    audio_registers_.set_NR52_internal(audio_registers_.get_NR52() & ~(1 << Traits::NR52_BIT));
  }

  void enable_channel() {
    enabled_ = CHANNEL_ENABLED;
    audio_registers_.set_NR52_internal(audio_registers_.get_NR52() | (1 << Traits::NR52_BIT));
  }

protected:
  float enabled_ = CHANNEL_DISABLED;
  float left_enabled_ = CHANNEL_DISABLED;
  float right_enabled_ = CHANNEL_DISABLED;
  AudioRegisters& audio_registers_;
  typename Traits::LengthTimerType length_timer_;
  typename Traits::DACType dac_;
};
