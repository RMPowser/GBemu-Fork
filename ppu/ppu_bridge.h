#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

struct PPUBridge {
  std::function<void()> trigger_vblank_interrupt;
  std::function<void()> trigger_lcd_stat_interrupt;
  std::function<void(const uint32_t* pixels, size_t pitch)> blit_screen;
  std::function<bool()> is_halted;  //Needed for correct handling of delaying interrupts in halted mode.
  std::function<const uint8_t*(uint16_t)> read_memory;  //Needed for OAM DMA transfers.
};
