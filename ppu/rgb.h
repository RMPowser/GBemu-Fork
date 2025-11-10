#pragma once

#include <inttypes.h>

struct RGBValue {
  uint32_t argb = 0xFF000000u;
  uint8_t index = 0;
};

constexpr uint32_t make_argb(uint8_t r, uint8_t g, uint8_t b) {
  return 0xFF000000u | (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) |
         static_cast<uint32_t>(b);
}

constexpr RGBValue make_rgb(uint8_t r, uint8_t g, uint8_t b) {
  return RGBValue{make_argb(r, g, b), 0};
}

struct RGBAValue : public RGBValue {
  RGBAValue() = default;
  RGBAValue(const RGBAValue&) = default;
  RGBAValue(RGBValue rgb, bool active, bool priority) : RGBValue(rgb), active(active), priority(priority) {}
  bool active = false;
  bool priority = false;
};
