#pragma once

#include <inttypes.h>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>
#include "bank_registers.h"
#include "constants.h"

class ROMLoader;
class CPURegisters;
class SaveStateSerializer;

// ===== Memory Controller Constants =====
namespace MemoryControllerConstants {
constexpr size_t ROM_BANK_SIZE = 1024 * 16;    // 16 KB
constexpr size_t RAM_BANK_SIZE = 1024 * 8;     // 8 KB
constexpr size_t WRAM_SIZE = 1024 * 8;         // 8 KB
constexpr size_t HRAM_SIZE = 127;              // 127 bytes
constexpr size_t BOOT_ROM_SIZE = 256;          // 256 bytes
constexpr uint32_t RAM_SAVE_INTERVAL = 20000;  // Save RAM every 20000 ticks
constexpr uint8_t RAM_DISABLED_VALUE = 0xFF;   // Value returned when RAM is disabled
}  // namespace MemoryControllerConstants

class MemoryController {
public:
  MemoryController(ROMLoader& rom_loader);

  void set_write_callback(std::function<void(uint16_t, uint8_t)> callback) { write_callback_ = callback; }

  [[gnu::always_inline]] void tick() {
    if (ramDirty_ && tickCount_++ % MemoryControllerConstants::RAM_SAVE_INTERVAL == 0 &&
        !ram_filename_.empty()) {
      std::cout << "MemoryController: Saving RAM data to " << ram_filename_ << std::endl;
      std::ofstream ramfile(ram_filename_, std::ios::binary);
      for (uint32_t i = 0; i < ram_bank_count_; ++i) {
        ramfile.write(reinterpret_cast<char*>(ramBanks_[i].data()), ramBanks_[i].size());
      }
      ramDirty_ = false;
    }
  }

  void refresh_bank_map() {
    ROMbank00_ = &memoryBanks_[registers_.get_rom0()];
    ROMbankNN_ = &memoryBanks_[registers_.get_rom1()];
    RAMbank_ = &ramBanks_[registers_.get_ram0()];
  }

  void unload_boot_rom() {
    bootROMActive_ = false;
    std::cout << "MemoryController: Unloaded boot ROM" << std::endl;
  }

  void write_callback(uint16_t addr, uint8_t value) {
    if (write_callback_)
      write_callback_(addr, value);
  }

  const uint8_t* read_rom0(uint16_t addr) const {
    if (bootROMActive_ && addr < ROM_START) {
      return &bootROM_[addr];
    } else {
      return &(*ROMbank00_)[addr];
    }
  }

  void write_rom0(uint16_t addr, uint8_t value) {
    registers_.write(addr, value);
    refresh_bank_map();
  }
  const uint8_t* read_rom1(uint16_t addr) const { return &(*ROMbankNN_)[addr - ROM1_START]; }
  void write_rom1(uint16_t addr, uint8_t value) {
    registers_.write(addr, value);
    refresh_bank_map();
  }

  const uint8_t* read_ram(uint16_t addr) const {
    static uint8_t garbage = MemoryControllerConstants::RAM_DISABLED_VALUE;
    if (!registers_.get_ram_enabled()) {
      return &garbage;
    } else {
      if (registers_.rom_type() == ROMType::MBC2) {
        addr = (addr & 0x1FF) + EXTERNAL_RAM_START;
      }
      return &(*RAMbank_)[addr - EXTERNAL_RAM_START];
    }
  }
  void write_ram(uint16_t addr, uint8_t value) {
    if (registers_.get_ram_enabled()) {
      if (registers_.rom_type() == ROMType::MBC2) {
        value |= UPPER_NIBBLE_MASK;
      }
      (*RAMbank_)[addr - EXTERNAL_RAM_START] = value;
      ramDirty_ = true;

      write_callback(addr, value);
    }
  }

  const uint8_t* read_wram(uint16_t addr) const { return &WRAM_[addr - WRAM_START]; }
  void write_wram(uint16_t addr, uint8_t value) { WRAM_[addr - WRAM_START] = value; }

  const uint8_t* read_hram(uint16_t addr) const { return &HRAM_[addr - HRAM_START]; }
  void write_hram(uint16_t addr, uint8_t value) { HRAM_[addr - HRAM_START] = value; }

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void initialise_ram();
  void load_rom(ROMLoader& loader);
  uint8_t mbc_type_;

  std::array<unsigned char, MemoryControllerConstants::ROM_BANK_SIZE>* ROMbank00_ = nullptr;
  std::array<unsigned char, MemoryControllerConstants::ROM_BANK_SIZE>* ROMbankNN_ = nullptr;
  std::array<unsigned char, MemoryControllerConstants::RAM_BANK_SIZE>* RAMbank_ = nullptr;

  std::array<unsigned char, MemoryControllerConstants::WRAM_SIZE> WRAM_;
  std::array<unsigned char, MemoryControllerConstants::HRAM_SIZE> HRAM_;
  std::vector<std::array<unsigned char, MemoryControllerConstants::ROM_BANK_SIZE>> memoryBanks_;
  std::array<std::array<unsigned char, MemoryControllerConstants::RAM_BANK_SIZE>, 4> ramBanks_;

  std::array<unsigned char, MemoryControllerConstants::BOOT_ROM_SIZE> bootROM_ = {0};
  bool bootROMActive_ = false;

  BankRegisters registers_;

  std::function<void(uint16_t, uint8_t)> write_callback_ = nullptr;

  bool ramDirty_ = false;
  uint8_t ram_bank_count_ = 0;
  std::string ram_filename_;
  uint32_t tickCount_ = 0;
};
