#pragma once

#include <inttypes.h>
#include "mbc.h"

class BankRegisters {
public:
  BankRegisters(uint32_t rom_bank_count, uint32_t ram_bank_count, ROMType rom_type);
  void write(uint16_t address, uint8_t value);

  uint32_t get_rom0();
  uint32_t get_rom1();
  uint32_t get_ram0();
  bool get_bankMode() const;
  bool get_ram_enabled() const;
  ROMType rom_type() const;

private:
  uint8_t bank1_ = 1;
  uint8_t bank2_ = 0;
  uint8_t bankRAM_ = 0;  //Only used for MBC5
  bool bankMode_ = false;
  bool ramEnabled_ = false;

  const uint8_t rom_bank_mask_;
  const uint8_t ram_bank_mask_;
  const ROMType rom_type_;
};