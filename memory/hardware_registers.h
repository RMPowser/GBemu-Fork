#pragma once

#include <inttypes.h>
#include <array>
#include <unordered_set>
#include "constants.h"
#include "utils.h"

// ===== Hardware Register Constants =====
namespace HardwareRegisterConstants {
// Register Offsets (from 0xFF00)
constexpr uint8_t P1_JOYP_OFFSET = 0x00;
constexpr uint8_t SB_OFFSET = 0x01;
constexpr uint8_t SC_OFFSET = 0x02;
constexpr uint8_t DIV_OFFSET = 0x04;
constexpr uint8_t TIMA_OFFSET = 0x05;
constexpr uint8_t TMA_OFFSET = 0x06;
constexpr uint8_t TAC_OFFSET = 0x07;
constexpr uint8_t IF_OFFSET = 0x0F;
constexpr uint8_t SVBK_WBK_OFFSET = 0x70;
constexpr uint8_t PCM12_OFFSET = 0x76;
constexpr uint8_t PCM34_OFFSET = 0x77;
constexpr uint8_t IE_OFFSET = 0xFF;

// Full Register Addresses
constexpr uint16_t P1_JOYP_ADDRESS = 0xFF00;
constexpr uint16_t SC_ADDRESS = 0xFF02;
constexpr uint16_t TAC_ADDRESS = 0xFF07;
constexpr uint16_t IF_ADDRESS = 0xFF0F;
constexpr uint16_t UNUSED_7D_ADDRESS = 0xFF7D;
constexpr uint16_t UNUSED_7E_ADDRESS = 0xFF7E;
constexpr uint16_t UNUSED_7F_ADDRESS = 0xFF7F;

// Default Register Values
constexpr uint8_t P1_JOYP_DEFAULT = 0xCF;
constexpr uint8_t SC_DEFAULTT = 0x7E;
constexpr uint8_t TAC_DEFAULT = 0xF8;
constexpr uint8_t IF_DEFAULT = 0xE1;

// Register Masks
constexpr uint8_t P1_JOYP_WRITE_MASK = 0x30;
constexpr uint8_t P1_JOYP_READ_MASK = 0xCF;
constexpr uint8_t P1_JOYP_SET_MASK = 0xC0;
constexpr uint8_t SC_MASK = 0x7E;
constexpr uint8_t TAC_MASK = 0xF8;
constexpr uint8_t IF_MASK = 0xE0;

// Address Ranges
constexpr uint16_t UNUSED_AUDIO_START = 0xFF27;
constexpr uint16_t UNUSED_AUDIO_END = 0xFF2F;
constexpr uint16_t AUDIO_REGISTERS_START = 0xFF10;
constexpr uint16_t AUDIO_REGISTERS_END = 0xFF3F;

constexpr size_t REGISTER_ARRAY_SIZE = 256;
}  // namespace HardwareRegisterConstants

class HardwareRegisters {
private:
  static constexpr uint16_t BASE_ADDRESS = HIGH_RAM_BASE;
  std::array<uint8_t, HardwareRegisterConstants::REGISTER_ARRAY_SIZE> regs_{};

public:
  HardwareRegisters() {
    // Initialize with default values
    regs_[HardwareRegisterConstants::P1_JOYP_OFFSET] = HardwareRegisterConstants::P1_JOYP_DEFAULT;
    regs_[HardwareRegisterConstants::SB_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::SC_OFFSET] = HardwareRegisterConstants::SC_DEFAULTT;
    regs_[HardwareRegisterConstants::DIV_OFFSET] = 0x00;  // Updated by clock
    regs_[HardwareRegisterConstants::TIMA_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::TMA_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::TAC_OFFSET] = HardwareRegisterConstants::TAC_DEFAULT;
    regs_[HardwareRegisterConstants::IF_OFFSET] = HardwareRegisterConstants::IF_DEFAULT;
    regs_[HardwareRegisterConstants::SVBK_WBK_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::PCM12_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::PCM34_OFFSET] = 0x00;
    regs_[HardwareRegisterConstants::IE_OFFSET] = 0x00;
  }

public:
  // Getter functions
  [[gnu::always_inline]] const uint8_t& get_P1_JOYP() const {
    return regs_[HardwareRegisterConstants::P1_JOYP_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_SB() const { return regs_[HardwareRegisterConstants::SB_OFFSET]; }
  [[gnu::always_inline]] const uint8_t& get_SC() const { return regs_[HardwareRegisterConstants::SC_OFFSET]; }
  [[gnu::always_inline]] const uint8_t& get_TIMA() const {
    return regs_[HardwareRegisterConstants::TIMA_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_TMA() const {
    return regs_[HardwareRegisterConstants::TMA_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_TAC() const {
    return regs_[HardwareRegisterConstants::TAC_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_IF() const { return regs_[HardwareRegisterConstants::IF_OFFSET]; }
  [[gnu::always_inline]] const uint8_t& get_IE() const { return regs_[HardwareRegisterConstants::IE_OFFSET]; }
  [[gnu::always_inline]] const uint8_t& get_SVBK_WBK() const {
    return regs_[HardwareRegisterConstants::SVBK_WBK_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_PCM12() const {
    return regs_[HardwareRegisterConstants::PCM12_OFFSET];
  }
  [[gnu::always_inline]] const uint8_t& get_PCM34() const {
    return regs_[HardwareRegisterConstants::PCM34_OFFSET];
  }

  [[gnu::always_inline]] void set_P1_JOYP(uint8_t value) {
    regs_[HardwareRegisterConstants::P1_JOYP_OFFSET] =
        (value & HardwareRegisterConstants::P1_JOYP_WRITE_MASK) |
        (regs_[HardwareRegisterConstants::P1_JOYP_OFFSET] & HardwareRegisterConstants::P1_JOYP_READ_MASK);
  }
  [[gnu::always_inline]] void set_P1_JOYP_internal(uint8_t value) {
    regs_[HardwareRegisterConstants::P1_JOYP_OFFSET] = value | HardwareRegisterConstants::P1_JOYP_SET_MASK;
  }
  [[gnu::always_inline]] void set_SB(uint8_t value) { regs_[HardwareRegisterConstants::SB_OFFSET] = value; }
  [[gnu::always_inline]] void set_SC(uint8_t value) {
    regs_[HardwareRegisterConstants::SC_OFFSET] = value | HardwareRegisterConstants::SC_MASK;
  }
  [[gnu::always_inline]] void set_TAC(uint8_t value) {
    regs_[HardwareRegisterConstants::TAC_OFFSET] = value | HardwareRegisterConstants::TAC_MASK;
  }
  [[gnu::always_inline]] void set_IF(uint8_t value) {
    regs_[HardwareRegisterConstants::IF_OFFSET] = value | HardwareRegisterConstants::IF_MASK;
  }
  [[gnu::always_inline]] void set_TIMA(uint8_t value) {
    regs_[HardwareRegisterConstants::TIMA_OFFSET] = value;
  }
  [[gnu::always_inline]] void set_TMA(uint8_t value) { regs_[HardwareRegisterConstants::TMA_OFFSET] = value; }

  [[gnu::always_inline]] void set_IE(uint8_t value) { regs_[HardwareRegisterConstants::IE_OFFSET] = value; }
  [[gnu::always_inline]] void set_SVBK_WBK(uint8_t value) {
    regs_[HardwareRegisterConstants::SVBK_WBK_OFFSET] = value;
  }
  [[gnu::always_inline]] void set_PCM12(uint8_t value) {
    regs_[HardwareRegisterConstants::PCM12_OFFSET] = value;
  }
  [[gnu::always_inline]] void set_PCM34(uint8_t value) {
    regs_[HardwareRegisterConstants::PCM34_OFFSET] = value;
  }
  [[gnu::always_inline]] void trigger_vblank_interrupt() {
    regs_[HardwareRegisterConstants::IF_OFFSET] |= VBLANK_INTERRUPT_FLAG;
  }
  [[gnu::always_inline]] void trigger_lcd_stat_interrupt() {
    regs_[HardwareRegisterConstants::IF_OFFSET] |= LCD_STAT_INTERRUPT_FLAG;
  }
  [[gnu::always_inline]] void trigger_timer_interrupt() {
    regs_[HardwareRegisterConstants::IF_OFFSET] |= TIMER_INTERRUPT_FLAG;
  }
  [[gnu::always_inline]] void trigger_serial_interrupt() {
    regs_[HardwareRegisterConstants::IF_OFFSET] |= SERIAL_INTERRUPT_FLAG;
  }
  [[gnu::always_inline]] void trigger_joypad_interrupt() {
    regs_[HardwareRegisterConstants::IF_OFFSET] |= JOYPAD_INTERRUPT_FLAG;
  }

  [[gnu::always_inline]] void write_register(uint16_t address, uint8_t value) {
    if (address >= HardwareRegisterConstants::UNUSED_AUDIO_START &&
        address <= HardwareRegisterConstants::UNUSED_AUDIO_END) {
      return;
    }

    // Audio registers should never reach HardwareRegisters
    if (address >= HardwareRegisterConstants::AUDIO_REGISTERS_START &&
        address <= HardwareRegisterConstants::AUDIO_REGISTERS_END) {
      FATAL("Audio register write should not reach HardwareRegisters: 0x" + std::to_string(address));
    }

    uint16_t offset = address - BASE_ADDRESS;

    // Special handling for registers with write behavior
    if (address == HardwareRegisterConstants::P1_JOYP_ADDRESS) {
      set_P1_JOYP(value);
    } else if (address == HardwareRegisterConstants::SC_ADDRESS) {
      set_SC(value);
    } else if (address == HardwareRegisterConstants::TAC_ADDRESS) {
      set_TAC(value);
    } else if (address == HardwareRegisterConstants::IF_ADDRESS) {
      set_IF(value);
    } else if (address == HardwareRegisterConstants::UNUSED_7D_ADDRESS ||
               address == HardwareRegisterConstants::UNUSED_7E_ADDRESS ||
               address == HardwareRegisterConstants::UNUSED_7F_ADDRESS) {
      return;
    } else {
      regs_[offset] = value;
    }
  }

  [[gnu::always_inline]] inline const uint8_t& read_register(uint16_t address) const {
    static const std::unordered_set<uint16_t> ignored_registers = {
        0xFF03, 0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF15, 0xFF1F, 0xFF27,
        0xFF28, 0xFF29, 0xFF6D, 0xFF6E, 0xFF6F, 0xFF70, 0xFF71, 0xFF72, 0xFF73, 0xFF74, 0xFF75,
        0xFF76, 0xFF77, 0xFF78, 0xFF79, 0xFF7A, 0xFF7B, 0xFF7C, 0xFF7D, 0xFF7E, 0xFF7F};

    static const uint8_t dummy = BYTE_MASK;
    if (ignored_registers.count(address)) {
      return dummy;
    }

    return regs_[address - BASE_ADDRESS];
  }
};
