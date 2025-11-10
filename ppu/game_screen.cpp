#include "game_screen.h"
#include "ppu_constants.h"
#include "rgb.h"

void GameScreen::clear(const RGBValue& color) {
  const uint32_t packed = color.argb;
  for (uint32_t y = 0; y < SCREEN_HEIGHT; ++y) {
    for (uint32_t x = 0; x < SCREEN_WIDTH; ++x) {
      background_pixels_[y][x] = color;
      pixel_buffer_[(y * SCREEN_WIDTH) + x] = packed;
    }
  }
}