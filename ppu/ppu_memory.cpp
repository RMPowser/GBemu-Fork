#include "ppu_memory.h"
#include <cstring>
#include "ppu_constants.h"
#include "save_state.h"

static_assert(VRAM_SIZE == 1024 * 8, "VRAM_SIZE must be 8192 bytes");
static_assert(OAM_SIZE == 160, "OAM_SIZE must be 160 bytes");

PPUMemory::PPUMemory(const PPURegisters& ppu_registers) : ppu_registers_(ppu_registers) {
  memset(vram_.data(), 0, sizeof(vram_));
  memset(oam_.data(), 0, sizeof(oam_));
}

void PPUMemory::tick() {
  // Tick OAMDMA transfers
  for (auto it = oam_dmas_.begin(); it != oam_dmas_.end();) {
    if (it->tick()) {
      it = oam_dmas_.erase(it);
      PPU_VERBOSE_PRINT() << "OAMDMA completed: " << oam_dmas_.size() << std::endl;
    } else {
      ++it;
    }
  }
}

const uint8_t* PPUMemory::read_oam(uint16_t addr) const {
  static uint8_t garbage = OAMDMA_GARBAGE_VALUE;
  const uint8_t ppu_mode = ppu_registers_.get_STAT() & STAT_MODE_MASK;
  if (ppu_mode == PPU_MODE_OAM_SEARCH || ppu_mode == PPU_MODE_PIXEL_TRANSFER || is_oam_dma_running()) {
    PPU_VERBOSE_PRINT() << "OAMDMA in progress, returning garbage for address: " << std::hex << addr
                        << std::dec << std::endl;
    return &garbage;
  }
  return &oam_[addr - OAM_BASE_ADDRESS];
}

void PPUMemory::write_oam(uint16_t addr, uint8_t value) {
  const uint8_t ppu_mode = ppu_registers_.get_STAT() & STAT_MODE_MASK;
  if (ppu_mode == PPU_MODE_OAM_SEARCH || ppu_mode == PPU_MODE_PIXEL_TRANSFER || is_oam_dma_running()) {
    return;
  }

  oam_[addr - OAM_BASE_ADDRESS] = value;
}

void PPUMemory::start_oamdma(std::function<const uint8_t*(uint16_t)> read_memory, uint16_t source_address) {
  oam_dmas_.emplace_back(read_memory, oam_, source_address);
}

void PPUMemory::serialize(SaveStateSerializer& serializer) const {
  serializer << vram_;
  serializer << oam_;
  serializer << oam_dmas_;
}

void PPUMemory::deserialize(SaveStateSerializer& serializer) {
  serializer >> vram_;
  serializer >> oam_;
  serializer >> oam_dmas_;
}

void PPUMemory::restore_oamdma_pointers(std::function<const uint8_t*(uint16_t)> read_memory) {
  for (auto& oam_dma : oam_dmas_) {
    oam_dma.restore_pointers(read_memory, oam_);
  }
}
