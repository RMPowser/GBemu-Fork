#pragma once

#include <cstdint>
#include "audio_constants.h"
#include "audio_registers.h"
#include "frame_sequencer.h"
#include "mixer.h"
#include "stack_vector.h"

class SaveStateSerializer;

class APU {
public:
  APU(std::function<void(const int16_t* samples, int num_samples)> sample_generated_callback);

  //Should be called once per m-cycle
  void tick();

  //Should be called every 2048 m-cycles - or more specifically, when the 10th bit falls on the internal clock
  void tick_frame_sequencer();

  //This doesn't need to be called, but should probably be called just before pausing for FPS management to clear the sample buffer.
  void generate_samples();

  //These are obvious, write and read everything from 0xFF10 to 0xFF3F here
  void audio_register_write(uint16_t address, uint8_t value);
  const unsigned char* audio_register_read(uint16_t address) const;

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void add_samples_to_buffer();
  void reset_registers();
  bool is_length_register(uint16_t address);

  std::function<void(const int16_t* samples, int num_samples)> on_samples_generated_;

  FrameSequencer frame_sequencer_;
  AudioRegisters audio_registers_;

  Mixer mixer_;
  bool master_enabled_ = true;

  constexpr static float M_CYCLES_PER_SAMPLE = 21.845333f;
  float sample_counter_ = M_CYCLES_PER_SAMPLE;

  StackVector<int16_t, SAMPLE_BUFFER_SIZE> samples_buffer_;
  uint32_t apu_clock_ = 0;
};
