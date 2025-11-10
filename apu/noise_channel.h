#pragma once

#include <cstdint>
#include "channel_base.h"
#include "channel_traits.h"
#include "dac.h"
#include "length_timer.h"

class FrameSequencer;
class AudioRegisters;

class NoiseChannel : public ChannelBase<NoiseChannel, Channel4Traits> {
public:
  NoiseChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers);
  void audio_register_write(uint16_t address, uint8_t value);
};