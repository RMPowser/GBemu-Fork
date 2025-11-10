#include "object_attributes.h"
#include <algorithm>
#include "ppu_constants.h"

ObjectAttributes::ObjectAttributes(std::array<unsigned char, OAM_SIZE>& oam) {
  attributes_ = reinterpret_cast<ObjectAttribute*>(&oam);
}

ObjectAttribute* ObjectAttributes::begin() {
  return attributes_;
}

ObjectAttribute* ObjectAttributes::end() {
  return attributes_ + TOTAL_SPRITES;
}

std::array<ObjectAttribute*, 10>& ObjectAttributes::get_objects_for_scanline(int16_t scanline,
                                                                             bool big_tile_mode) {
  current_objects.fill(nullptr);

  uint8_t next_object = 0;
  for (uint32_t i = 0; i < TOTAL_SPRITES; ++i) {
    if (object_interacts_with_scanline(attributes_[i], scanline, big_tile_mode)) {
      current_objects[next_object] = &attributes_[i];
      next_object++;
    }

    if (next_object == MAX_SPRITES_PER_SCANLINE)
      break;
  }

  sort_by_x(current_objects);

  return current_objects;
}

bool ObjectAttributes::object_interacts_with_scanline(ObjectAttribute& object, int16_t scanline,
                                                      bool big_tile_mode) {
  int16_t y_top = object.y - SPRITE_Y_OFFSET;
  constexpr int16_t Y_BOTTOM_OFFSET_8X8 = 9;
  constexpr int16_t Y_BOTTOM_OFFSET_8X16 = 1;
  int16_t y_bottom = object.y - (big_tile_mode ? Y_BOTTOM_OFFSET_8X16 : Y_BOTTOM_OFFSET_8X8);

  if (scanline > y_bottom || scanline < y_top)
    return false;
  return true;
}

void ObjectAttributes::sort_by_x(std::array<ObjectAttribute*, 10>& objects) {
  std::stable_sort(objects.begin(), objects.end(), [](ObjectAttribute* a, ObjectAttribute* b) {
    if (a == nullptr)
      return false;  // nullptr goes to end
    if (b == nullptr)
      return true;  // non-nullptr comes before nullptr
    return a->x < b->x;
  });
}