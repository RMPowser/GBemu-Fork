#pragma once

#include <array>
#include <cstdint>
#include "ppu_constants.h"

namespace {
constexpr uint8_t REGISTER_OFFSET_LCDC = 0x00;
constexpr uint8_t REGISTER_OFFSET_STAT = 0x01;
constexpr uint8_t REGISTER_OFFSET_SCY = 0x02;
constexpr uint8_t REGISTER_OFFSET_SCX = 0x03;
constexpr uint8_t REGISTER_OFFSET_LY = 0x04;
constexpr uint8_t REGISTER_OFFSET_LYC = 0x05;
constexpr uint8_t REGISTER_OFFSET_DMA = 0x06;
constexpr uint8_t REGISTER_OFFSET_BGP = 0x07;
constexpr uint8_t REGISTER_OFFSET_OBP0 = 0x08;
constexpr uint8_t REGISTER_OFFSET_OBP1 = 0x09;
constexpr uint8_t REGISTER_OFFSET_WY = 0x0A;
constexpr uint8_t REGISTER_OFFSET_WX = 0x0B;

constexpr uint8_t LCDC_DEFAULT = 0x91;
constexpr uint8_t STAT_DEFAULT = 0x85;
constexpr uint8_t BGP_DEFAULT = 0xFC;
constexpr uint8_t OBP0_DEFAULT = 0xFF;
constexpr uint8_t OBP1_DEFAULT = 0xFF;
constexpr uint8_t REGISTER_READ_DUMMY = 0xFF;
constexpr uint16_t PPU_REGISTER_SIZE = 0x2D;
}  // namespace

class PPURegisters {
public:
  PPURegisters(bool boot_rom_active) {
    if (!boot_rom_active) {
      regs_[REGISTER_OFFSET_LCDC] = LCDC_DEFAULT;
      regs_[REGISTER_OFFSET_STAT] = STAT_DEFAULT;
      regs_[REGISTER_OFFSET_BGP] = BGP_DEFAULT;
      regs_[REGISTER_OFFSET_OBP0] = OBP0_DEFAULT;
      regs_[REGISTER_OFFSET_OBP1] = OBP1_DEFAULT;
    }
  }

  // Getter functions
  [[gnu::always_inline]] const uint8_t& get_LCDC() const { return regs_[REGISTER_OFFSET_LCDC]; }
  [[gnu::always_inline]] const uint8_t& get_STAT() const { return regs_[REGISTER_OFFSET_STAT]; }
  [[gnu::always_inline]] const uint8_t& get_SCY() const { return regs_[REGISTER_OFFSET_SCY]; }
  [[gnu::always_inline]] const uint8_t& get_SCX() const { return regs_[REGISTER_OFFSET_SCX]; }
  [[gnu::always_inline]] const uint8_t& get_LY() const { return regs_[REGISTER_OFFSET_LY]; }
  [[gnu::always_inline]] const uint8_t& get_LYC() const { return regs_[REGISTER_OFFSET_LYC]; }
  [[gnu::always_inline]] const uint8_t& get_DMA() const { return regs_[REGISTER_OFFSET_DMA]; }
  [[gnu::always_inline]] const uint8_t& get_BGP() const { return regs_[REGISTER_OFFSET_BGP]; }
  [[gnu::always_inline]] const uint8_t& get_OBP0() const { return regs_[REGISTER_OFFSET_OBP0]; }
  [[gnu::always_inline]] const uint8_t& get_OBP1() const { return regs_[REGISTER_OFFSET_OBP1]; }
  [[gnu::always_inline]] const uint8_t& get_WY() const { return regs_[REGISTER_OFFSET_WY]; }
  [[gnu::always_inline]] const uint8_t& get_WX() const { return regs_[REGISTER_OFFSET_WX]; }

  // Setter functions
  [[gnu::always_inline]] void set_STAT(uint8_t value) {
    regs_[REGISTER_OFFSET_STAT] =
        (value & STAT_WRITABLE_MASK) | (regs_[REGISTER_OFFSET_STAT] & STAT_READONLY_MASK) | STAT_BIT_7_SET;
  }
  [[gnu::always_inline]] void set_STAT_internal(uint8_t value) {
    regs_[REGISTER_OFFSET_STAT] = value | STAT_BIT_7_SET;
  }

  [[gnu::always_inline]] void write_register(uint16_t address, uint8_t value) {
    uint16_t offset = address - PPU_REGISTER_START;
    if (address == STAT_ADDR) {
      set_STAT(value);
    } else {
      regs_[offset] = value;
    }
  }

  [[gnu::always_inline]] const uint8_t& read_register(uint16_t address) const {
    // Many PPU registers are write-only or not implemented
    static const std::array<bool, PPU_REGISTER_SIZE> readable = {
        true,                                                           // 0xFF40 - LCDC (readable)
        true,                                                           // 0xFF41 - STAT (readable)
        true,                                                           // 0xFF42 - SCY (readable)
        true,                                                           // 0xFF43 - SCX (readable)
        true,                                                           // 0xFF44 - LY (readable)
        true,                                                           // 0xFF45 - LYC (readable)
        true,                                                           // 0xFF46 - DMA (readable)
        true,                                                           // 0xFF47 - BGP (readable)
        true,                                                           // 0xFF48 - OBP0 (readable)
        true,                                                           // 0xFF49 - OBP1 (readable)
        true,                                                           // 0xFF4A - WY (readable)
        true,                                                           // 0xFF4B - WX (readable)
        false,                                                          // 0xFF4C - KEY0_SYS (not implemented)
        false,                                                          // 0xFF4D - KEY1_SPD (not implemented)
        false,                                                          // 0xFF4E - unmapped
        false,                                                          // 0xFF4F - VBK (not implemented)
        false,                                                          // 0xFF50 - BANK (not implemented)
        false,                                                          // 0xFF51 - HDMA1 (not implemented)
        false,                                                          // 0xFF52 - HDMA2 (not implemented)
        false,                                                          // 0xFF53 - HDMA3 (not implemented)
        false,                                                          // 0xFF54 - HDMA4 (not implemented)
        false,                                                          // 0xFF55 - HDMA5 (not implemented)
        false,                                                          // 0xFF56 - RP (not implemented)
        false, false, false, false, false, false, false, false, false,  // 0xFF57-0xFF5F
        false, false, false, false, false, false, false, false,         // 0xFF60-0xFF67
        false,  // 0xFF68 - BCPS_BGPI (not implemented)
        false,  // 0xFF69 - BCPD_BGPD (not implemented)
        false,  // 0xFF6A - OCPS_OBPI (not implemented)
        false,  // 0xFF6B - OCPD_OBPD (not implemented)
        false   // 0xFF6C - OPRI (not implemented)
    };

    static const uint8_t dummy = REGISTER_READ_DUMMY;
    const uint16_t offset = address - PPU_REGISTER_START;

    if (offset >= regs_.size() || !readable[offset]) {
      return dummy;
    }

    return regs_[offset];
  }

private:
  std::array<uint8_t, PPU_REGISTER_SIZE> regs_ = {0};  // 0xFF40-0xFF6C (45 bytes)
};
