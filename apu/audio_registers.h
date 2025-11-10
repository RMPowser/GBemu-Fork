#pragma once

#include <cstdint>
#include "audio_constants.h"

class AudioRegisters {
public:
  // Contiguous array for registers 0xFF10-0xFF26 (23 bytes)
  // registers_[0] = 0xFF10, registers_[22] = 0xFF26
  uint8_t registers_[23] = {
      0x80,  // 0xFF10 - NR10 - Channel 1 sweep
      0xBF,  // 0xFF11 - NR11 - Channel 1 length timer & duty cycle
      0xF3,  // 0xFF12 - NR12 - Channel 1 volume & envelope
      0xFF,  // 0xFF13 - NR13 - Channel 1 period low (write-only)
      0xBF,  // 0xFF14 - NR14 - Channel 1 period high & control
      0xFF,  // 0xFF15 - NR20 - Channel 2 unused
      0x3F,  // 0xFF16 - NR21 - Channel 2 length timer & duty cycle
      0x00,  // 0xFF17 - NR22 - Channel 2 volume & envelope
      0xFF,  // 0xFF18 - NR23 - Channel 2 period low (write-only)
      0xBF,  // 0xFF19 - NR24 - Channel 2 period high & control
      0x7F,  // 0xFF1A - NR30 - Channel 3 DAC enable
      0xFF,  // 0xFF1B - NR31 - Channel 3 length timer (write-only)
      0x9F,  // 0xFF1C - NR32 - Channel 3 output level
      0xFF,  // 0xFF1D - NR33 - Channel 3 period low (write-only)
      0xBF,  // 0xFF1E - NR34 - Channel 3 period high & control
      0xFF,  // 0xFF1F - NR40 - Channel 4 unused
      0xFF,  // 0xFF20 - NR41 - Channel 4 length timer (write-only)
      0x00,  // 0xFF21 - NR42 - Channel 4 volume & envelope
      0x00,  // 0xFF22 - NR43 - Channel 4 polynomial counter
      0xBF,  // 0xFF23 - NR44 - Channel 4 counter & control
      0x77,  // 0xFF24 - NR50 - Sound panning
      0xF3,  // 0xFF25 - NR51 - Sound output selection
      0xF1   // 0xFF26 - NR52 - Audio master control
  };

  // Wave Pattern RAM ($FF30-$FF3F)
  uint8_t wave_ram_[WAVE_RAM_SIZE] = {0};  // $FF30-$FF3F - Wave pattern data
  mutable uint32_t last_wave_ram_access_ = 0;

  // Getter functions
  [[gnu::always_inline]] const uint8_t& get_NR10() const { return registers_[0x00]; }  // 0xFF10
  [[gnu::always_inline]] const uint8_t& get_NR11() const { return registers_[0x01]; }  // 0xFF11
  [[gnu::always_inline]] const uint8_t& get_NR12() const { return registers_[0x02]; }  // 0xFF12
  [[gnu::always_inline]] const uint8_t& get_NR13() const { return registers_[0x03]; }  // 0xFF13
  [[gnu::always_inline]] const uint8_t& get_NR14() const { return registers_[0x04]; }  // 0xFF14
  [[gnu::always_inline]] const uint8_t& get_NR20() const { return registers_[0x05]; }  // 0xFF15
  [[gnu::always_inline]] const uint8_t& get_NR21() const { return registers_[0x06]; }  // 0xFF16
  [[gnu::always_inline]] const uint8_t& get_NR22() const { return registers_[0x07]; }  // 0xFF17
  [[gnu::always_inline]] const uint8_t& get_NR23() const { return registers_[0x08]; }  // 0xFF18
  [[gnu::always_inline]] const uint8_t& get_NR24() const { return registers_[0x09]; }  // 0xFF19
  [[gnu::always_inline]] const uint8_t& get_NR30() const { return registers_[0x0A]; }  // 0xFF1A
  [[gnu::always_inline]] const uint8_t& get_NR31() const { return registers_[0x0B]; }  // 0xFF1B
  [[gnu::always_inline]] const uint8_t& get_NR32() const { return registers_[0x0C]; }  // 0xFF1C
  [[gnu::always_inline]] const uint8_t& get_NR33() const { return registers_[0x0D]; }  // 0xFF1D
  [[gnu::always_inline]] const uint8_t& get_NR34() const { return registers_[0x0E]; }  // 0xFF1E
  [[gnu::always_inline]] const uint8_t& get_NR40() const { return registers_[0x0F]; }  // 0xFF1F
  [[gnu::always_inline]] const uint8_t& get_NR41() const { return registers_[0x10]; }  // 0xFF20
  [[gnu::always_inline]] const uint8_t& get_NR42() const { return registers_[0x11]; }  // 0xFF21
  [[gnu::always_inline]] const uint8_t& get_NR43() const { return registers_[0x12]; }  // 0xFF22
  [[gnu::always_inline]] const uint8_t& get_NR44() const { return registers_[0x13]; }  // 0xFF23
  [[gnu::always_inline]] const uint8_t& get_NR50() const { return registers_[0x14]; }  // 0xFF24
  [[gnu::always_inline]] const uint8_t& get_NR51() const { return registers_[0x15]; }  // 0xFF25
  [[gnu::always_inline]] const uint8_t& get_NR52() const { return registers_[0x16]; }  // 0xFF26
  [[gnu::always_inline]] const uint8_t& get_WAVE_RAM(uint8_t index) const { return wave_ram_[index]; }
  [[gnu::always_inline]] const uint8_t& get_WAVE_RAM_internal(uint8_t index, uint32_t apu_clock) const {
    last_wave_ram_access_ = apu_clock;
    return wave_ram_[index];
  }

  // Setter functions
  [[gnu::always_inline]] void set_NR10(uint8_t value) { registers_[0x00] = value | 0x80; }  // 0xFF10
  [[gnu::always_inline]] void set_NR11(uint8_t value) { registers_[0x01] = value | 0x3F; }  // 0xFF11
  [[gnu::always_inline]] void set_NR12(uint8_t value) { registers_[0x02] = value; }         // 0xFF12
  [[gnu::always_inline]] void set_NR13(uint8_t value) { registers_[0x03] = value | 0xFF; }  // 0xFF13
  [[gnu::always_inline]] void set_NR14(uint8_t value) { registers_[0x04] = value | 0xBF; }  // 0xFF14
  [[gnu::always_inline]] void set_NR20(uint8_t value) { registers_[0x05] = value | 0xFF; }  // 0xFF15
  [[gnu::always_inline]] void set_NR21(uint8_t value) { registers_[0x06] = value | 0x3F; }  // 0xFF16
  [[gnu::always_inline]] void set_NR22(uint8_t value) { registers_[0x07] = value; }         // 0xFF17
  [[gnu::always_inline]] void set_NR23(uint8_t value) { registers_[0x08] = value | 0xFF; }  // 0xFF18
  [[gnu::always_inline]] void set_NR24(uint8_t value) { registers_[0x09] = value | 0xBF; }  // 0xFF19
  [[gnu::always_inline]] void set_NR30(uint8_t value) { registers_[0x0A] = value | 0x7F; }  // 0xFF1A
  [[gnu::always_inline]] void set_NR31(uint8_t value) { registers_[0x0B] = value | 0xFF; }  // 0xFF1B
  [[gnu::always_inline]] void set_NR32(uint8_t value) { registers_[0x0C] = value | 0x9F; }  // 0xFF1C
  [[gnu::always_inline]] void set_NR33(uint8_t value) { registers_[0x0D] = value | 0xFF; }  // 0xFF1D
  [[gnu::always_inline]] void set_NR34(uint8_t value) { registers_[0x0E] = value | 0xBF; }  // 0xFF1E
  [[gnu::always_inline]] void set_NR40(uint8_t value) { registers_[0x0F] = value | 0xFF; }  // 0xFF1F
  [[gnu::always_inline]] void set_NR41(uint8_t value) { registers_[0x10] = value | 0xFF; }  // 0xFF20
  [[gnu::always_inline]] void set_NR42(uint8_t value) { registers_[0x11] = value; }         // 0xFF21
  [[gnu::always_inline]] void set_NR43(uint8_t value) { registers_[0x12] = value; }         // 0xFF22
  [[gnu::always_inline]] void set_NR44(uint8_t value) { registers_[0x13] = value | 0xBF; }  // 0xFF23
  [[gnu::always_inline]] void set_NR50(uint8_t value) { registers_[0x14] = value; }         // 0xFF24
  [[gnu::always_inline]] void set_NR51(uint8_t value) { registers_[0x15] = value; }         // 0xFF25
  [[gnu::always_inline]] void set_NR52(uint8_t value) {
    registers_[0x16] = (value & 0x80) | (registers_[0x16] & 0x0F) | 0x70;
  }  // 0xFF26
  [[gnu::always_inline]] void set_NR52_internal(uint8_t value) {
    registers_[0x16] = (value & 0x8F) | 0x70;
  }  // 0xFF26
  [[gnu::always_inline]] void set_WAVE_RAM(uint8_t index, uint8_t value) {
    if (index < WAVE_RAM_SIZE)
      wave_ram_[index] = value;
  }

  // Direct array access for addresses $FF10-$FF3F
  [[gnu::always_inline]] void write_register(uint16_t address, uint8_t value) {
    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
      set_WAVE_RAM(address - WAVE_RAM_START, value);
    } else if (address >= AUDIO_REG_START && address <= AUDIO_REG_END) {
      const uint16_t offset = address - AUDIO_REG_START;
      static constexpr uint8_t masks[23] = {
          0x80, 0x3F, 0x00, 0xFF, 0xBF,  // NR10-NR14
          0xFF, 0x3F, 0x00, 0xFF, 0xBF,  // NR20-NR24
          0x7F, 0xFF, 0x9F, 0xFF, 0xBF,  // NR30-NR34
          0xFF, 0xFF, 0x00, 0x00, 0xBF,  // NR40-NR44
          0x00, 0x00, 0x00               // NR50-NR52 (special case)
      };
      if (offset == 0x16) {  // NR52 special case
        set_NR52(value);
      } else {
        registers_[offset] = value | masks[offset];
      }
    }
  }

  [[gnu::always_inline]] const uint8_t& read_register(uint16_t address) const {
    static const uint8_t dummy = 0xFF;

    if (address >= WAVE_RAM_START && address <= WAVE_RAM_END) {
      // Wave Pattern RAM
      return get_WAVE_RAM(address - WAVE_RAM_START);
    } else if (address >= AUDIO_REG_START && address <= AUDIO_REG_END) {
      // Regular registers - direct array access
      return registers_[address - AUDIO_REG_START];
    }

    return dummy;
  }
};