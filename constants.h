#pragma once

#include <inttypes.h>

// ===== General Settings =====
constexpr bool VERBOSE = false;
constexpr bool TIMER_VERBOSE = false;
// constexpr bool VERBOSE = false;
// constexpr bool TIMER_VERBOSE = false;
constexpr bool MEMORY_VERBOSE = false;

// ===== Memory Map Addresses =====
constexpr uint16_t ROM_START = 0x100;
constexpr uint16_t ROM0_START = 0x0000;
constexpr uint16_t ROM0_END = 0x3FFF;
constexpr uint16_t ROM1_START = 0x4000;
constexpr uint16_t ROM1_END = 0x7FFF;
constexpr uint16_t VRAM_START = 0x8000;
constexpr uint16_t VRAM_END = 0x9FFF;
constexpr uint16_t EXTERNAL_RAM_START = 0xA000;
constexpr uint16_t EXTERNAL_RAM_END = 0xBFFF;
constexpr uint16_t WRAM_START = 0xC000;
constexpr uint16_t WRAM_END = 0xDFFF;
constexpr uint16_t ECHO_RAM_START = 0xE000;
constexpr uint16_t ECHO_RAM_END = 0xFDFF;
constexpr uint16_t OAM_START = 0xFE00;
constexpr uint16_t OAM_END = 0xFE9F;
constexpr uint16_t PROHIBITED_START = 0xFEA0;
constexpr uint16_t PROHIBITED_END = 0xFEFF;
constexpr uint16_t IO_REGISTERS_START = 0xFF00;
constexpr uint16_t IO_REGISTERS_END = 0xFF7F;
constexpr uint16_t HRAM_START = 0xFF80;
constexpr uint16_t HRAM_END = 0xFFFE;
constexpr uint16_t IE_REGISTER = 0xFFFF;

// ===== Hardware Register Offsets =====
constexpr uint16_t ECHO_RAM_OFFSET = 0x2000;
constexpr uint16_t MEMORY_BRIDGE_OFFSET = 0xFE00;
constexpr uint16_t HIGH_RAM_BASE = 0xFF00;

// ===== Interrupt Vectors =====
constexpr uint16_t VBLANK_VECTOR = 0x40;
constexpr uint16_t LCD_STAT_VECTOR = 0x48;
constexpr uint16_t TIMER_VECTOR = 0x50;
constexpr uint16_t SERIAL_VECTOR = 0x58;
constexpr uint16_t JOYPAD_VECTOR = 0x60;

// ===== Interrupt Flags/Masks =====
constexpr uint8_t VBLANK_INTERRUPT_FLAG = 0x01;
constexpr uint8_t LCD_STAT_INTERRUPT_FLAG = 0x02;
constexpr uint8_t TIMER_INTERRUPT_FLAG = 0x04;
constexpr uint8_t SERIAL_INTERRUPT_FLAG = 0x08;
constexpr uint8_t JOYPAD_INTERRUPT_FLAG = 0x10;
constexpr uint8_t ALL_INTERRUPTS_MASK = 0x1F;

// ===== CPU Flag Bits =====
constexpr uint8_t ZERO_FLAG_BIT = 0x80;
constexpr uint8_t SUBTRACTION_FLAG_BIT = 0x40;
constexpr uint8_t HALF_CARRY_FLAG_BIT = 0x20;
constexpr uint8_t CARRY_FLAG_BIT = 0x10;

// ===== Bit Masks for Arithmetic =====
constexpr uint8_t LOWER_NIBBLE_MASK = 0x0F;
constexpr uint8_t UPPER_NIBBLE_MASK = 0xF0;
constexpr uint8_t BYTE_MASK = 0xFF;
constexpr uint16_t LOWER_12_BITS_MASK = 0x0FFF;
constexpr uint16_t WORD_MASK = 0xFFFF;

// ===== Halt States =====
enum HALT_STATE { NO_HALT, HALT, HALT_BUG };