#pragma once

#include <inttypes.h>
#include <array>
#include "ppu_constants.h"

namespace {
constexpr uint8_t FLAG_PRIORITY_BIT = 7;
constexpr uint8_t FLAG_FLIP_Y_BIT = 6;
constexpr uint8_t FLAG_FLIP_X_BIT = 5;
constexpr uint8_t FLAG_PALETTE_BIT = 4;
}  // namespace

class ObjectAttribute {
public:
  uint8_t y;
  uint8_t x;
  uint8_t index;
  uint8_t flags;

  bool priority() const { return (flags >> FLAG_PRIORITY_BIT) & 1; }
  bool flip_x() const { return (flags >> FLAG_FLIP_X_BIT) & 1; }
  bool flip_y() const { return (flags >> FLAG_FLIP_Y_BIT) & 1; }
  bool dmg_palette_obp1() const { return (flags >> FLAG_PALETTE_BIT) & 1; }
};

class ObjectAttributes {
public:
  ObjectAttributes(std::array<unsigned char, OAM_SIZE>& oam);
  ObjectAttribute* begin();
  ObjectAttribute* end();

  std::array<ObjectAttribute*, MAX_SPRITES_PER_SCANLINE>& get_objects_for_scanline(int16_t scanline,
                                                                                   bool big_tile_mode);

private:
  bool object_interacts_with_scanline(ObjectAttribute& object, int16_t scanline, bool big_tile_mode);
  void sort_by_x(std::array<ObjectAttribute*, MAX_SPRITES_PER_SCANLINE>& objects);
  ObjectAttribute* attributes_;

  std::array<ObjectAttribute*, MAX_SPRITES_PER_SCANLINE> current_objects{nullptr};
};