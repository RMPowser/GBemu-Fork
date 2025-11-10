#include "apu.h"
#include <cstdint>
#include "audio_constants.h"
#include "save_state.h"

// Sample conversion constant
constexpr float SAMPLE_RANGE_MAX = 32767.0f;

APU::APU(std::function<void(const int16_t* samples, int num_samples)> sample_generated_callback)
    : mixer_(frame_sequencer_, audio_registers_) {
  on_samples_generated_ = sample_generated_callback;
}

void APU::audio_register_write(uint16_t address, uint8_t value) {
  if (address == NR26_ADDR) {
    audio_registers_.write_register(address, value);
    const bool previous_master_enabled = master_enabled_;
    const bool new_master_enabled = value & MASTER_ENABLE_BIT;

    if (previous_master_enabled != new_master_enabled) {
      if (new_master_enabled) {
        frame_sequencer_.reset();
        mixer_.master_enable();
      } else {
        reset_registers();
      }
    }

    master_enabled_ = new_master_enabled;
    return;
  }

  //If we're not enabled, writes are ignored except for length registers.
  if (master_enabled_) {
    //Wave ram writes are only allowed if we're disabled, or if we accessed wave ram this cycle (and if we did, we can only write to the last slot read).
    if (mixer_.channel3().enabled() && address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
      if (audio_registers_.last_wave_ram_access_ == apu_clock_) {
        audio_registers_.set_WAVE_RAM(mixer_.channel3().ram_position() >> 1, value);
      }
    } else {
      audio_registers_.write_register(address, value);
      mixer_.audio_register_write(address, value);
    }
  } else if (is_length_register(address)) {
    //We're only allowed to write length counter bits, not duty, when powered off.
    if (address == NR11_ADDR || address == NR16_ADDR)
      value &= LENGTH_COUNTER_MASK;
    audio_registers_.write_register(address, value);
    mixer_.audio_register_write(address, value);
  }
}

const unsigned char* APU::audio_register_read(uint16_t address) const {
  static uint8_t garbage = 0xFF;

  //If wave ram is enabled, we can only write to the last slot read if wave ram was accessed on exactly this clock.
  //This is mostly just to pass a test ROM, but it's a good idea to do it anyway.
  if (mixer_.channel3().enabled() && address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
    if (audio_registers_.last_wave_ram_access_ == apu_clock_) {
      return &audio_registers_.get_WAVE_RAM(mixer_.channel3().ram_position() >> 1);
    } else {
      return &garbage;
    }
  }

  return &audio_registers_.read_register(address);
}

void APU::tick_frame_sequencer() {
  frame_sequencer_.tick();
}

void APU::tick() {
  apu_clock_++;
  mixer_.tick(apu_clock_);

  sample_counter_ -= 1.0f;
  if (sample_counter_ <= 0.0f) {

    sample_counter_ += M_CYCLES_PER_SAMPLE;
    add_samples_to_buffer();
  }
}

void APU::add_samples_to_buffer() {
  auto [left, right] = mixer_.output();

  // Convert float samples (range -1.0 to 1.0) to int16_t (range -32768 to 32767)
  samples_buffer_.push_back(static_cast<int16_t>(left * SAMPLE_RANGE_MAX));
  samples_buffer_.push_back(static_cast<int16_t>(right * SAMPLE_RANGE_MAX));

  if (samples_buffer_.size() == SAMPLE_BUFFER_SIZE) {
    generate_samples();
  }
}

void APU::generate_samples() {
  on_samples_generated_(samples_buffer_.data(), samples_buffer_.size());
  samples_buffer_.clear();
}

void APU::reset_registers() {
  // Save the length counter bits from NRx1 registers (bits 0-5)
  uint8_t channel1_length = audio_registers_.get_NR11() & LENGTH_COUNTER_MASK;
  uint8_t channel2_length = audio_registers_.get_NR21() & LENGTH_COUNTER_MASK;
  uint8_t channel3_length = audio_registers_.get_NR31() & WAVE_LENGTH_MASK;
  uint8_t channel4_length = audio_registers_.get_NR41() & LENGTH_COUNTER_MASK;

  // Reset all registers to 0 using audio_register_write
  for (uint16_t i = AUDIO_REG_START; i < AUDIO_REG_END; i++) {
    audio_register_write(i, 0);
  }

  // Restore the length counter bits
  audio_register_write(NR11_ADDR, channel1_length);
  audio_register_write(NR16_ADDR, channel2_length);
  audio_register_write(NR1B_ADDR, channel3_length);
  audio_register_write(NR20_ADDR, channel4_length);

  // Reset NR52 to 0
  audio_registers_.set_NR52_internal(0);
}

bool APU::is_length_register(uint16_t address) {
  return address == NR11_ADDR || address == NR16_ADDR || address == NR1B_ADDR || address == NR20_ADDR;
}

void APU::serialize(SaveStateSerializer& serializer) const {
  serializer << audio_registers_;
  serializer << master_enabled_;
}

void APU::deserialize(SaveStateSerializer& serializer) {
  serializer >> audio_registers_;
  serializer >> master_enabled_;

  audio_register_write(NR26_ADDR, audio_registers_.read_register(NR26_ADDR));
  for (uint16_t i = AUDIO_REG_START; i < AUDIO_REG_END; i++) {
    audio_register_write(i, audio_registers_.read_register(i));
  }
}