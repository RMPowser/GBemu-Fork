#include "ppu.h"
#include <cstdint>
#include <numeric>
#include "palette.h"
#include "ppu_constants.h"
#include "rgb.h"
#include "save_state.h"

namespace {
constexpr uint8_t PIXEL_BIT_SHIFT = 7;

[[gnu::always_inline]] inline uint8_t get_half_pixel(uint8_t data, uint8_t pixel) {
  return (data >> (PIXEL_BIT_SHIFT - pixel)) & 1;
}

[[gnu::always_inline]] inline uint8_t get_pixel(const uint8_t* data, uint8_t pixel) {
  return get_half_pixel(data[0], pixel) | get_half_pixel(data[1], pixel) << 1;
}

bool stat_should_fire(uint8_t current_stat) {
  return (((current_stat & STAT_LYC_INT) && (current_stat & STAT_LYC_FLAG)) ||
          ((current_stat & STAT_OAM_INT) && ((current_stat & STAT_MODE_MASK) == PPU_MODE_OAM_SEARCH)) ||
          ((current_stat & STAT_VBLANK_INT) && ((current_stat & STAT_MODE_MASK) == PPU_MODE_VBLANK)) ||
          ((current_stat & STAT_HBLANK_INT) && ((current_stat & STAT_MODE_MASK) == PPU_MODE_HBLANK)));
}

}  // namespace

PPU::PPU(PPUBridge ppu_bridge, bool boot_rom_active)
    : ppu_registers_(boot_rom_active),
      ppu_memory_(ppu_registers_),
      oam_attributes_(ppu_memory_.oam()),
      palette_(ppu_registers_),
      ppu_bridge_(std::move(ppu_bridge)) {
  enabled_ = LCDC() & LCDC_DISPLAY_ENABLE;
}

void PPU::tick() {
  ppu_memory_.tick();

  if (!enabled_)
    return;

  // This is the key to hblank_ly_scx_timing-GS.s
  if (fire_hblank_next_tick_) {
    PPU_VERBOSE_PRINT() << "PPU: Firing HBlank interrupt" << std::endl;
    fire_hblank_next_tick_ = false;
    ppu_bridge_.trigger_lcd_stat_interrupt();
  }

  elapsed_t_cycles_ += T_CYCLES_PER_TICK;
  check_mode_change();
}

uint16_t PPU::get_bg_tile_map_address(uint32_t x, uint32_t y) {
  const uint8_t tile_map = (LCDC() & LCDC_BG_TILE_MAP) ? 1 : 0;
  const uint16_t tile_map_base_address = tile_map == 0 ? TILE_MAP_BASE_0 : TILE_MAP_BASE_1;
  uint8_t tile_x = x / TILE_WIDTH;
  uint8_t tile_y = y / TILE_HEIGHT;
  return tile_map_base_address + ((tile_y * TILES_PER_ROW) + tile_x);
}

uint16_t PPU::get_win_tile_map_address(uint32_t x, uint32_t y) {
  const uint8_t tile_map = (LCDC() & LCDC_WINDOW_TILE_MAP) ? 1 : 0;
  const uint16_t tile_map_base_address = tile_map == 0 ? TILE_MAP_BASE_0 : TILE_MAP_BASE_1;
  return tile_map_base_address + ((y / TILE_HEIGHT) * TILES_PER_ROW + (x / TILE_WIDTH));
}

uint16_t PPU::get_bgwin_tile_address(uint16_t tile_index) {
  const uint8_t tile_addressing_mode = (ppu_registers_.get_LCDC() & LCDC_TILE_DATA) ? 1 : 0;
  if (tile_addressing_mode == 1) {
    return TILE_DATA_BASE_0 + (tile_index * TILE_SIZE_BYTES);
  } else {
    const int8_t signed_index = (int8_t)tile_index;
    return TILE_DATA_BASE_1 + (signed_index * TILE_SIZE_BYTES);
  }
}

uint16_t PPU::get_obj_tile_address(uint16_t tile_index) {
  return TILE_DATA_BASE_0 + (tile_index * TILE_SIZE_BYTES);
}

uint8_t PPU::get_object_mode_3_penalty(std::array<ObjectAttribute*, 10>& objects, uint8_t scx) {
  std::array<int16_t, 21> penalty_map = {0};

  scx &= 7;
  uint8_t total = scx;

  for (auto object : objects) {
    if (object == nullptr)
      break;

    uint8_t object_x = object->x;

    if (object_x >= 168)
      continue;

    if (object_x == 0) {
      object_x += scx;
    }

    uint8_t bucket = object_x >> 3;

    penalty_map[bucket] = std::max(penalty_map[bucket], static_cast<int16_t>(5 - (object_x & 7)));
    total += 6;
  }

  total +=
      std::accumulate(penalty_map.begin(), penalty_map.end(), 0, [](int16_t a, int16_t b) { return a + b; });
  return (total >> 2) * 4;
}

void PPU::render_object_scanline(uint8_t scanline) {
  if ((LCDC() & LCDC_SPRITE_ENABLE) == 0) {
    for (int32_t x = 0; x < (int32_t)SCREEN_WIDTH; x++) {
      game_screen_.draw_object_pixel(x, scanline, {GameBoyColors::BLACK, false, false});
    }
    return;
  }

  const bool big_tile_mode = (LCDC() & LCDC_SPRITE_SIZE) != 0;

  std::array<ObjectAttribute*, 10> objects =
      oam_attributes_.get_objects_for_scanline(scanline, big_tile_mode);

  mode_3_penalty_ += get_object_mode_3_penalty(objects, ppu_registers_.get_SCX());

  for (int32_t x = 0; x < (int32_t)SCREEN_WIDTH; x++) {
    bool object_found = false;
    for (auto object : objects) {
      if (object == nullptr)
        break;

      const int16_t object_left_position = object->x - SPRITE_X_OFFSET;
      const int16_t object_right_position = object->x - 1;  // Rightmost pixel
      const int16_t object_top_position = object->y - SPRITE_Y_OFFSET;
      if (x < object_left_position || x > object_right_position)
        continue;  // Not in range, check next object

      object_found = true;

      uint32_t pixel_x = (int16_t)x - object_left_position;
      uint32_t pixel_y = (int16_t)scanline - object_top_position;

      // Handle flip X/Y
      if (object->flip_x())
        pixel_x = (TILE_PIXELS_PER_ROW - 1) - pixel_x;
      if (object->flip_y()) {
        uint8_t sprite_height = big_tile_mode ? SPRITE_HEIGHT_8X16 : SPRITE_HEIGHT_8X8;
        pixel_y = sprite_height - pixel_y;
      }

      uint8_t tile_index = object->index;
      if (big_tile_mode) {
        // In 8x16 mode, bit 0 is ignored and two consecutive tiles are used
        constexpr uint8_t TILE_INDEX_BIT_0_MASK = 0xFE;
        constexpr uint8_t TILE_INDEX_BIT_0_SET = 0x01;
        tile_index &= TILE_INDEX_BIT_0_MASK;  // Clear bit 0
        if (pixel_y >= TILE_HEIGHT) {
          tile_index |= TILE_INDEX_BIT_0_SET;  // Use second tile for bottom half
          pixel_y -= TILE_HEIGHT;
        }
      }

      const uint16_t tile_address = get_obj_tile_address(tile_index);
      constexpr uint8_t BYTES_PER_TILE_ROW = 2;
      const uint16_t tile_row_address = tile_address + (pixel_y * BYTES_PER_TILE_ROW);
      const uint8_t* byte1 = &ppu_memory_.vram()[tile_row_address - VRAM_BASE_ADDRESS];
      const uint8_t colour_index = get_pixel(byte1, pixel_x);

      bool use_obp1 = object->dmg_palette_obp1();
      game_screen_.draw_object_pixel(x, scanline,
                                     palette_.get_object_color(colour_index, object->priority(), use_obp1));

      constexpr uint8_t TRANSPARENT_COLOR_INDEX = 0;
      if (colour_index == TRANSPARENT_COLOR_INDEX) {
        continue;
      }

      break;  // Found object for this pixel, stop checking others
    }

    if (!object_found) {
      game_screen_.draw_object_pixel(x, scanline, {GameBoyColors::BLACK, false, false});
    }
  }
}

void PPU::render_background_scanline(uint8_t scanline) {
  if ((LCDC() & LCDC_BG_ENABLE) == 0) {
    for (uint32_t x = 0; x < SCREEN_WIDTH; x++) {
      game_screen_.draw_background_pixel(x, scanline, GameBoyColors::WHITE);
    }
    return;
  }

  bool window_enabled = (LCDC() & LCDC_WINDOW_ENABLE) != 0;
  const uint8_t wx = ppu_registers_.get_WX();
  const uint8_t wy = ppu_registers_.get_WY();

  // Check if window is visible on this scanline
  const bool window_visible_on_scanline = window_enabled && (scanline >= wy);
  if (window_visible_on_scanline && wx < WINDOW_MAX_X) {
    mode_3_penalty_ += MODE_3_WINDOW_SWITCH_PENALTY;
    window_scanline_++;
  }

  const uint8_t scx = ppu_registers_.get_SCX();
  const uint8_t scy = ppu_registers_.get_SCY();

  const uint32_t window_y = window_scanline_ - 1;
  const uint32_t background_y = (scanline + scy) & 0xFF;

  // Render only the specified scanline
  for (uint32_t x = 0; x < SCREEN_WIDTH; x++) {
    uint16_t map_x, map_y;
    uint16_t tile_map_address;

    // Check if we should render window or background at this pixel
    // Window X position is WX - 7, and window is only visible if x + 7 >= WX
    if (window_visible_on_scanline && (x + WINDOW_X_OFFSET >= wx)) {
      // Render window pixel
      // Window has its own internal coordinate system starting at (0,0)
      const uint16_t window_x = x + WINDOW_X_OFFSET - wx;

      tile_map_address = get_win_tile_map_address(window_x, window_y);
      map_x = window_x;
      map_y = window_y;
    } else {
      // Render background pixel
      map_x = (x + scx) & 0xFF;
      map_y = background_y;
      tile_map_address = get_bg_tile_map_address(map_x, map_y);
    }

    const uint8_t tile_index = ppu_memory_.vram()[tile_map_address - VRAM_BASE_ADDRESS];
    const uint16_t tile_data_address = get_bgwin_tile_address(tile_index);

    // Pixel position within the 8x8 tile
    const uint16_t pixel_x = map_x % TILE_WIDTH;
    const uint16_t pixel_y = map_y % TILE_HEIGHT;

    // Each row is 2 bytes
    constexpr uint8_t BYTES_PER_TILE_ROW = 2;
    const uint16_t tile_row_address = tile_data_address + (pixel_y * BYTES_PER_TILE_ROW);
    const uint8_t* byte1 = &ppu_memory_.vram()[tile_row_address - VRAM_BASE_ADDRESS];
    const uint8_t colour_index = get_pixel(byte1, pixel_x);
    game_screen_.draw_background_pixel(x, scanline, palette_.bg_colors()[colour_index]);
  }
}

void PPU::render_scanline(uint8_t scanline) {
  render_background_scanline(scanline);
  render_object_scanline(scanline);
}

bool PPU::frame_completed() {
  if (frame_just_completed_) {
    frame_just_completed_ = false;
    return true;
  }
  return false;
}

void PPU::check_mode_change() {
  switch (current_mode_) {
    case PPUMode::OAMSearch:
      if (elapsed_t_cycles_ >= OAM_SEARCH_CYCLES) {
        set_mode(PPUMode::PixelTransfer);
        elapsed_t_cycles_ -= OAM_SEARCH_CYCLES;
      }
      break;
    case PPUMode::PixelTransfer:
      if (elapsed_t_cycles_ >= (PIXEL_TRANSFER_BASE_CYCLES + mode_3_penalty_)) {
        elapsed_t_cycles_ -= (PIXEL_TRANSFER_BASE_CYCLES + mode_3_penalty_);
        set_mode(PPUMode::HBlank);
      }
      break;
    case PPUMode::HBlank:
      if (elapsed_t_cycles_ >= (HBLANK_BASE_CYCLES - mode_3_penalty_)) {
        elapsed_t_cycles_ -= (HBLANK_BASE_CYCLES - mode_3_penalty_);
        mode_3_penalty_ = 0;

        if (just_enabled_) {
          just_enabled_ = false;
          set_mode(PPUMode::PixelTransfer);
          break;
        } else {
          scanline_++;
          set_LY();
        }

        if (scanline_ == VBLANK_START_LINE) {
          ppu_bridge_.trigger_vblank_interrupt();
          if (ppu_registers_.get_STAT() & STAT_OAM_INT) {
            fire_stat_interrupt(stat_interrupt_line_, true);
          }
          set_mode(PPUMode::VBlank);
        } else {
          set_mode(PPUMode::OAMSearch);
        }
      }
      break;
    case PPUMode::VBlank:
      if (elapsed_t_cycles_ >= SCANLINE_CYCLES) {
        scanline_++;
        elapsed_t_cycles_ -= SCANLINE_CYCLES;
        set_LY();
      }
      if (scanline_ == VBLANK_END_LINE) {
        scanline_ = 0;
        set_mode(PPUMode::OAMSearch);
        set_LY();
      }
      break;
  }
}

void PPU::set_mode(PPUMode mode) {
  PPU_VERBOSE_PRINT() << "PPU: Setting mode: " << static_cast<int>(mode) << std::endl;
  current_mode_ = mode;
  uint8_t new_stat =
      (ppu_registers_.get_STAT() & STAT_MODE_CLEAR_MASK) | static_cast<std::underlying_type_t<PPUMode>>(mode);
  stat_write(STAT_ADDR, new_stat);
  ppu_registers_.set_STAT_internal(new_stat);

  switch (mode) {
    case PPUMode::OAMSearch:
      break;
    case PPUMode::PixelTransfer:
      mode_3_penalty_ += ppu_registers_.get_SCX() % MODE_3_SCX_PENALTY_DIVISOR;
      render_scanline(scanline_);
      break;
    case PPUMode::HBlank:
      break;
    case PPUMode::VBlank:
      window_scanline_ = 0;          // Reset window line counter for next frame
      frame_just_completed_ = true;  // Signal that a frame has been completed
      ppu_bridge_.blit_screen(game_screen_.pixel_data(), game_screen_.pitch());
      break;
  }
}

void PPU::set_LY(bool force) {
  const uint16_t old_ly = ppu_registers_.get_LY();
  if (scanline_ != old_ly || force) {
    stat_write(LY_ADDR, scanline_);
    ppu_registers_.write_register(LY_ADDR, scanline_);
  }
}

void PPU::fire_stat_interrupt(bool previous_stat_should_fire, bool stat_interrupt_line) {
  if (!previous_stat_should_fire && stat_interrupt_line) {
    if (is_halted_hblank_interrupt()) {
      fire_hblank_next_tick_ = true;
      return;
    }

    PPU_VERBOSE_PRINT() << "PPU: Firing Interrupt" << std::endl;
    ppu_bridge_.trigger_lcd_stat_interrupt();
  }
}

void PPU::stat_write(uint16_t address, uint8_t value) {
  if (!enabled_ && !just_enabled_) {
    return;
  }

  const uint8_t LY = address == LY_ADDR ? value : ppu_registers_.get_LY();
  const uint8_t LYC = address == LYC_ADDR ? value : ppu_registers_.get_LYC();

  if (LY == LYC) {
    ppu_registers_.set_STAT_internal(ppu_registers_.get_STAT() | STAT_LYC_FLAG);
  } else {
    ppu_registers_.set_STAT_internal(ppu_registers_.get_STAT() & ~STAT_LYC_FLAG);
  }

  PPU_VERBOSE_PRINT() << "PPU: Stat write: " << std::hex << address << " = " << static_cast<int>(value)
                      << std::dec << std::endl;

  uint8_t current_stat = address == STAT_ADDR ? value : ppu_registers_.get_STAT();

  bool previous_stat_should_fire = stat_interrupt_line_;
  stat_interrupt_line_ = stat_should_fire(current_stat);
  PPU_VERBOSE_PRINT() << "PPU: Stat interrupt line: " << stat_interrupt_line_ << std::endl;

  fire_stat_interrupt(previous_stat_should_fire, stat_interrupt_line_);
}

void PPU::write_ppu_register(uint16_t addr, uint8_t value) {
  switch (addr) {
    case STAT_ADDR: {
      const uint8_t current_stat = *read_ppu_register(STAT_ADDR);
      stat_write(addr, (value & STAT_WRITABLE_MASK) | (current_stat & STAT_READONLY_MASK) | STAT_BIT_7_SET);
      break;
    }
    case LY_ADDR:
    case LYC_ADDR:
      stat_write(addr, value);
      break;
    case 0xFF46: {
      uint16_t source_address = value * 0x100;
      if (source_address >= 0xFE00) {
        source_address = ((source_address - 1) & 0x1000) | (source_address & 0xFFF) | 0xC000;
      }
      // Create lambda to read memory through the CPU's memory bridge
      ppu_memory_.start_oamdma(
          [this](uint16_t address) -> const uint8_t* { return ppu_bridge_.read_memory(address); },
          source_address);
      break;
    }

    default:
      break;
  }

  ppu_registers_.write_register(addr, value);

  switch (addr) {
    case BGP_ADDR:
      palette_.refresh_bg_colors();
      break;
    case LCDC_ADDR: {
      bool enabled = value & LCDC_DISPLAY_ENABLE;
      if (enabled && !enabled_) {
        scanline_ = 0;
        elapsed_t_cycles_ = PPU_ENABLE_OFFSET_CYCLES;

        just_enabled_ = true;
        set_LY(true);

        PPU_VERBOSE_PRINT() << "PPU just enabled: " << elapsed_t_cycles_ << std::endl;
      } else if (!enabled && enabled_) {
        set_mode(PPUMode::HBlank);
      }

      enabled_ = enabled;
    } break;
    default:
      break;
  }
}

bool PPU::is_halted_hblank_interrupt() const {
  return (current_mode_ == PPUMode::HBlank && ppu_bridge_.is_halted() &&
          (ppu_registers_.get_STAT() & STAT_HBLANK_INT));
}

void PPU::serialize(SaveStateSerializer& serializer) const {
  serializer << current_mode_;
  serializer << elapsed_t_cycles_;
  serializer << scanline_;
  serializer << mode_3_penalty_;
  serializer << window_scanline_;
  serializer << enabled_;
  serializer << just_enabled_;
  serializer << frame_just_completed_;
  serializer << stat_interrupt_line_;
  serializer << ppu_registers_;
  serializer << ppu_memory_;
}

void PPU::deserialize(SaveStateSerializer& serializer) {
  serializer >> current_mode_;
  serializer >> elapsed_t_cycles_;
  serializer >> scanline_;
  serializer >> mode_3_penalty_;
  serializer >> window_scanline_;
  serializer >> enabled_;
  serializer >> just_enabled_;
  serializer >> frame_just_completed_;
  serializer >> stat_interrupt_line_;
  serializer >> ppu_registers_;
  serializer >> ppu_memory_;
  palette_.refresh_bg_colors();

  ppu_memory_.restore_oamdma_pointers(
      [this](uint16_t address) -> const uint8_t* { return ppu_bridge_.read_memory(address); });
}