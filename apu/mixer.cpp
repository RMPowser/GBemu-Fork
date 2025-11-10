#include "mixer.h"
#include "audio_constants.h"

Mixer::Mixer(FrameSequencer& frame_sequencer, AudioRegisters& audio_registers)
    : channel1_(frame_sequencer, audio_registers),
      channel2_(frame_sequencer, audio_registers),
      channel3_(frame_sequencer, audio_registers),
      channel4_(frame_sequencer, audio_registers) {}

void Mixer::audio_register_write(uint16_t address, uint8_t value) {
  if (address == NR24_ADDR) {                                      // NR50 - Master Volume
    left_volume_ = static_cast<float>((value >> 4) & 0x7) / 7.0f;  // Bits 6-4
    right_volume_ = static_cast<float>(value & 0x7) / 7.0f;        // Bits 2-0
  }

  channel1_.audio_register_write(address, value);
  channel2_.audio_register_write(address, value);
  channel3_.audio_register_write(address, value);
  channel4_.audio_register_write(address, value);
}

void Mixer::master_enable() {
  channel1_.master_enable();
  channel2_.master_enable();
  channel3_.master_enable();
  channel4_.master_enable();
}

const WaveChannel& Mixer::channel3() const {
  return channel3_;
}