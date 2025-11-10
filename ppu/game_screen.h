#pragma once

#include <inttypes.h>
#include <array>
#include "ppu_constants.h"
#include "rgb.h"

class GameScreen {
public:
  [[gnu::always_inline]] inline void draw_background_pixel(uint32_t x, uint32_t y, const RGBValue& color) {
    background_pixels_[y][x] = color;
    pixel_buffer_[(y * SCREEN_WIDTH) + x] = color.argb;
  }

  [[gnu::always_inline]] inline void draw_object_pixel(uint32_t x, uint32_t y, const RGBAValue& color) {
    if (!color.active)
      return;

    const auto& background = background_pixels_[y][x];
    if (color.priority && background.index != 0)
      return;

    pixel_buffer_[(y * SCREEN_WIDTH) + x] = color.argb;
  }

  [[gnu::always_inline]] const uint32_t* pixel_data() const { return pixel_buffer_.data(); }

  [[gnu::always_inline]] constexpr static size_t pitch() { return SCREEN_WIDTH * sizeof(uint32_t); }

  void clear(const RGBValue& color);

private:
  std::array<std::array<RGBValue, SCREEN_WIDTH>, SCREEN_HEIGHT> background_pixels_;
  std::array<uint32_t, SCREEN_WIDTH * SCREEN_HEIGHT> pixel_buffer_{};
};