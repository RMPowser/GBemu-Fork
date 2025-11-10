#pragma once

#include <inttypes.h>
#include <array>
#include <functional>
#include "hardware_registers.h"

class SaveStateSerializer;

// ===== Timer Constants =====
namespace TimerConstants {
constexpr uint16_t INITIAL_CLOCK_VALUE = 0x2AF3;  // Correct state after CGB boot ROM
constexpr uint8_t DIV_SHIFT_AMOUNT = 6;           // Internal clock >> 6 to get DIV
constexpr uint8_t APU_FRAME_SEQUENCER_BIT = 10;   // APU frame sequencer runs every 8192 t-cycles
constexpr uint8_t TAC_ENABLE_BIT = 2;             // Bit in TAC that enables timer
constexpr uint8_t TAC_FREQUENCY_MASK = 0x03;      // Lower 2 bits select frequency
}  // namespace TimerConstants

class Timer {
public:
  Timer(HardwareRegisters& registers);
  void tick();
  void write_div();

  void write_tma(uint8_t value);
  void write_tima(uint8_t value);
  void write_tac(uint8_t value);

  const uint8_t* get_div() const;

  void set_apu_callback(const std::function<void()>& callback);

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void trigger_tima();
  void update_div();
  bool bit_fallen(uint16_t before, uint16_t after, uint16_t bit);
  void check_tima_trigger(uint16_t previous_internal_clock);

  // TAC frequency map: bit positions in internal clock for each TAC mode
  constexpr static std::array<uint16_t, 4> tac_map_ = {7, 1, 3, 5};
  HardwareRegisters& registers_;

  uint16_t internal_clock_ = 0;
  uint8_t div_ = 0;
  bool tima_written_this_cycle_ = false;
  bool trigger_tima_next_cycle_ = false;

  std::function<void()> apu_callback_;
};
