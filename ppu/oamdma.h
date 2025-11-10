#pragma once

#include <array>
#include <functional>
#include <iostream>
#include "ppu_constants.h"
#include "save_state.h"

class OAMDMA {
public:
  OAMDMA() = default;
  OAMDMA(std::function<const uint8_t*(uint16_t)> read_memory, std::array<unsigned char, OAM_SIZE>& oam,
         uint16_t source_address)
      : read_memory_(read_memory), oam_(&oam), source_address_(source_address) {}

  bool tick() {
    if (wait_) {
      wait_--;
      return false;
    }

    PPU_VERBOSE_PRINT() << "Copying OAM from Source: " << std::hex << source_address_ + index_ << " to "
                        << index_ << std::dec << std::endl;

    uint16_t address = source_address_ + index_;

    if (address >= OAM_BASE_ADDRESS && address <= OAM_END_ADDRESS) {
      (*oam_)[index_] = (*oam_)[address - OAM_BASE_ADDRESS];
    } else {
      (*oam_)[index_] = *read_memory_(address);
    }

    index_++;
    return index_ == oam_->size();
  }

  bool running() const { return wait_ == 0; }

  void serialize(SaveStateSerializer& serializer) const {
    serializer << source_address_;
    serializer << index_;
    serializer << wait_;
  }

  void deserialize(SaveStateSerializer& serializer) {
    serializer >> source_address_;
    serializer >> index_;
    serializer >> wait_;
  }

  void restore_pointers(std::function<const uint8_t*(uint16_t)> read_memory,
                        std::array<unsigned char, OAM_SIZE>& oam) {
    read_memory_ = read_memory;
    oam_ = &oam;
  }

private:
  std::function<const uint8_t*(uint16_t)> read_memory_;
  std::array<unsigned char, OAM_SIZE>* oam_;
  uint16_t source_address_;
  uint16_t index_ = 0;
  int wait_ = OAMDMA_WAIT_CYCLES;
};