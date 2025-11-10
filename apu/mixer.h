#pragma once

#include <cstdint>
#include <utility>
#include "channel_traits.h"
#include "frame_sequencer.h"
#include "noise_channel.h"
#include "square_wave_channel.h"
#include "wave_channel.h"

class AudioRegisters;

class Mixer {
public:
  Mixer(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers);

  void audio_register_write(uint16_t address, uint8_t value);
  void master_enable();
  const WaveChannel& channel3() const;

  std::pair<float, float> output() const {
    const auto [left1, right1] = channel1_.output();
    const auto [left2, right2] = channel2_.output();
    const auto [left3, right3] = channel3_.output();
    const auto [left4, right4] = channel4_.output();

    const float left = (left1 + left2 + left3 + left4) * left_volume_;
    const float right = (right1 + right2 + right3 + right4) * right_volume_;

    return {left, right};
  }

  [[gnu::always_inline]] void tick(uint32_t apu_clock) {
    channel1_.tick(apu_clock);
    channel2_.tick(apu_clock);
    channel3_.tick(apu_clock);
    channel4_.tick(apu_clock);
  }

private:
  SquareWaveChannel<Channel1Traits> channel1_;
  SquareWaveChannel<Channel2Traits> channel2_;
  WaveChannel channel3_;
  NoiseChannel channel4_;

  float left_volume_ = 1.0f;   // NR50 bits 6-4
  float right_volume_ = 1.0f;  // NR50 bits 2-0
};