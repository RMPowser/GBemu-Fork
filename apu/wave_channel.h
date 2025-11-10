#pragma once

#include <cstdint>
#include "channel_base.h"
#include "channel_traits.h"
#include "dac.h"
#include "length_timer.h"

class FrameSequencer;
class AudioRegisters;

class WaveChannel : public ChannelBase<WaveChannel, Channel3Traits> {
public:
  WaveChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers);
  void audio_register_write(uint16_t address, uint8_t value);
  uint8_t ram_position() const;

private:
  void corrupt_wave_ram();
};
