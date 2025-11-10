#include "memory_controller.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "constants.h"
#include "rom_loader.h"
#include "save_state.h"
#include "utils.h"

MemoryController::MemoryController(ROMLoader& loader)
    : registers_(loader.rom_bank_count(), loader.ram_bank_count(), loader.rom_type()) {

  mbc_type_ = loader.header()->cart_type;

  if (loader.should_initialise_mbc()) {
    initialise_ram();
    load_rom(loader);
  }
}

//Todo this should be pseudo-uninitialised
void MemoryController::initialise_ram() {
  memset(WRAM_.data(), 0, sizeof(WRAM_));
  memset(HRAM_.data(), 0, sizeof(HRAM_));
}

void MemoryController::load_rom(ROMLoader& loader) {
  if (loader.has_boot_rom()) {
    memcpy(bootROM_.data(), loader.boot_rom_data(), MemoryControllerConstants::BOOT_ROM_SIZE);
    bootROMActive_ = true;
  }

  auto rom_bank_count = loader.rom_bank_count();
  auto ram_bank_count = loader.ram_bank_count();
  std::cout << "Loading " << rom_bank_count << " ROM banks" << std::endl;
  std::cout << "Loading " << ram_bank_count << " RAM banks" << std::endl;

  memoryBanks_.resize(rom_bank_count);
  ROMbank00_ = &memoryBanks_[0];
  ROMbankNN_ = &memoryBanks_[1];
  RAMbank_ = &ramBanks_[0];

  for (uint32_t i = 0; i < memoryBanks_.size(); ++i) {
    memcpy(memoryBanks_[i].data(), loader.data(i * MemoryControllerConstants::ROM_BANK_SIZE),
           MemoryControllerConstants::ROM_BANK_SIZE);
  }

  if (loader.has_battery()) {
    ram_filename_ = loader.ram_filename();
    ram_bank_count_ = loader.ram_bank_count();

    if (loader.ram_size() == ram_bank_count_ * MemoryControllerConstants::RAM_BANK_SIZE) {
      std::cout << "MemoryController: Loading " << loader.ram_size() << " bytes of RAM data" << std::endl;
      for (uint32_t i = 0; i < ram_bank_count; ++i) {
        memcpy(ramBanks_[i].data(), loader.ram_data(i * MemoryControllerConstants::RAM_BANK_SIZE),
               MemoryControllerConstants::RAM_BANK_SIZE);
      }
    }
  }

  // Hexdump of all memory banks
  if constexpr (MEMORY_VERBOSE) {
    for (uint32_t i = 0; i < memoryBanks_.size(); ++i) {
      VERBOSE_PRINT() << "Bank " << i << " hexdump:" << std::endl;
      for (size_t j = 0; j < memoryBanks_[i].size(); j += 16) {
        VERBOSE_PRINT() << std::hex << std::setw(4) << std::setfill('0') << j << ": ";
        for (size_t k = 0; k < 16 && (j + k) < memoryBanks_[i].size(); ++k) {
          VERBOSE_PRINT() << std::setw(2) << std::setfill('0') << static_cast<int>(memoryBanks_[i][j + k])
                          << " ";
        }
        VERBOSE_PRINT() << std::endl;
      }
      VERBOSE_PRINT() << std::dec;  // Reset to decimal output
    }
  }
}

void MemoryController::serialize(SaveStateSerializer& serializer) const {
  serializer << registers_;
  serializer << mbc_type_;
  serializer << tickCount_;
  serializer << WRAM_;
  serializer << HRAM_;
  serializer << memoryBanks_;
  serializer << ramBanks_;
  serializer << ram_filename_;
  serializer << ram_bank_count_;

  //Need to reset ROMbank00_ and ROMbankNN_ and RAMbank_ pointers
}

void MemoryController::deserialize(SaveStateSerializer& serializer) {
  serializer >> registers_;
  serializer >> mbc_type_;
  serializer >> tickCount_;
  serializer >> WRAM_;
  serializer >> HRAM_;
  serializer >> memoryBanks_;
  serializer >> ramBanks_;
  serializer >> ram_filename_;
  serializer >> ram_bank_count_;

  refresh_bank_map();
}
