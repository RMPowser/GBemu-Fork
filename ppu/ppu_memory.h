#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include "oamdma.h"
#include "ppu_registers.h"
#include "stack_vector.h"

class SaveStateSerializer;

class PPUMemory {
public:
  PPUMemory(const PPURegisters& ppu_registers);

  void tick();

  // VRAM access
  const uint8_t* read_vram(uint16_t addr) const { return &vram_[addr - VRAM_BASE_ADDRESS]; }
  void write_vram(uint16_t addr, uint8_t value) { vram_[addr - VRAM_BASE_ADDRESS] = value; }

  // OAM access
  const uint8_t* read_oam(uint16_t addr) const;
  void write_oam(uint16_t addr, uint8_t value);

  // OAMDMA management
  void start_oamdma(std::function<const uint8_t*(uint16_t)> read_memory, uint16_t source_address);
  bool is_oam_dma_running() const { return !oam_dmas_.empty() && oam_dmas_.front().running(); }

  // Direct access for sub-components
  const std::array<unsigned char, VRAM_SIZE>& vram() const { return vram_; }
  std::array<unsigned char, OAM_SIZE>& oam() { return oam_; }

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);
  void restore_oamdma_pointers(std::function<const uint8_t*(uint16_t)> read_memory);

private:
  std::array<unsigned char, VRAM_SIZE> vram_;
  std::array<unsigned char, OAM_SIZE> oam_;
  StackVector<OAMDMA, OAM_DMA_MAX_COUNT> oam_dmas_;
  const PPURegisters& ppu_registers_;
};
