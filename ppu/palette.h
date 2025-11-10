#pragma once

#include <inttypes.h>
#include <array>
#include "ppu_registers.h"
#include "rgb.h"

namespace GameBoyColors {
constexpr RGBValue WHITE = make_rgb(255, 255, 255);
constexpr RGBValue LIGHT_GRAY = make_rgb(170, 170, 170);
constexpr RGBValue DARK_GRAY = make_rgb(85, 85, 85);
constexpr RGBValue BLACK = make_rgb(0, 0, 0);
}  // namespace GameBoyColors

class Palette {
public:
  Palette(PPURegisters& ppu_registers) : ppu_registers_(ppu_registers) {}

  [[gnu::always_inline]] inline RGBValue get_color(uint8_t color_index) {
    return get_color_internal(color_index, ppu_registers_.get_BGP());
  }

  [[gnu::always_inline]] RGBAValue get_object_color(uint8_t color_index, bool priority, bool use_obp1) {
    const auto palette = use_obp1 ? ppu_registers_.get_OBP1() : ppu_registers_.get_OBP0();
    return {get_color_internal(color_index, palette), color_index == 0 ? false : true, priority};
  }

  void refresh_bg_colors() { bg_colors_ = {get_color(0), get_color(1), get_color(2), get_color(3)}; }
  std::array<RGBValue, 4>& bg_colors() { return bg_colors_; }

private:
  [[gnu::always_inline]] inline RGBValue get_color_internal(uint8_t color_index, uint8_t palette) {
    const uint8_t shade_id = (palette >> (color_index * 2)) & 0x03;
    RGBValue color = colors_[shade_id];
    color.index = color_index;
    return color;
  }

  PPURegisters& ppu_registers_;
  constexpr static std::array<RGBValue, 4> colors_ = {GameBoyColors::WHITE, GameBoyColors::LIGHT_GRAY,
                                                      GameBoyColors::DARK_GRAY, GameBoyColors::BLACK};
  std::array<RGBValue, 4> bg_colors_ = {GameBoyColors::WHITE, GameBoyColors::LIGHT_GRAY,
                                        GameBoyColors::DARK_GRAY, GameBoyColors::BLACK};
};