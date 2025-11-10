#pragma once

#include "channel_base.h"
#include "dac.h"
#include "length_timer.h"
#include "sweep.h"

class FrameSequencer;
class AudioRegisters;

template <typename Traits>
class SquareWaveChannel : public ChannelBase<SquareWaveChannel<Traits>, Traits> {
public:
  SquareWaveChannel(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers);

  void audio_register_write(uint16_t address, uint8_t value);

private:
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::disable_channel;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::enable_channel;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::audio_registers_;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::left_enabled_;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::right_enabled_;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::enabled_;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::length_timer_;
  using ChannelBase<SquareWaveChannel<Traits>, Traits>::dac_;

  Sweep sweep_;
};
