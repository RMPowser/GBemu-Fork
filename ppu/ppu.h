#pragma once

#include <inttypes.h>
#include "game_screen.h"
#include "object_attributes.h"
#include "palette.h"
#include "ppu_bridge.h"
#include "ppu_memory.h"
#include "ppu_registers.h"

class SaveStateSerializer;

enum class PPUMode : uint8_t {
  HBlank = 0,        // Mode 0
  VBlank = 1,        // Mode 1
  OAMSearch = 2,     // Mode 2
  PixelTransfer = 3  // Mode 3
};

class PPU {
public:
  PPU(PPUBridge ppu_bridge, bool boot_rom_active);

  //Call this once per m-cycle
  void tick();

  //Call this once per m-cycle to check if a frame is ready to be rendered (You will also have just got a call on the PPUBridge to blit the screen)
  bool frame_completed();

  //Read VRAM from here
  const uint8_t* read_vram(uint16_t addr) const {
    static uint8_t garbage = 0xFF;
    if (current_mode_ != PPUMode::PixelTransfer) {
      return ppu_memory_.read_vram(addr);
    }
    return &garbage;
  }

  //Write VRAM to here
  void write_vram(uint16_t addr, uint8_t value) {
    if (current_mode_ != PPUMode::PixelTransfer) {
      ppu_memory_.write_vram(addr, value);
    }
  }

  //Read OAM from here
  const uint8_t* read_oam(uint16_t addr) const { return ppu_memory_.read_oam(addr); }

  //Write OAM to here
  void write_oam(uint16_t addr, uint8_t value) { ppu_memory_.write_oam(addr, value); }

  //Read PPU registers from here - Everything from FF40 -> FF6C
  const uint8_t* read_ppu_register(uint16_t addr) const { return &ppu_registers_.read_register(addr); }

  //Write PPU registers to here - Everything from FF40 -> FF6C
  void write_ppu_register(uint16_t addr, uint8_t value);

  //Saving and loading PPU state.
  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void stat_write(uint16_t address, uint8_t value);
  void check_mode_change();

  uint8_t get_object_mode_3_penalty(std::array<ObjectAttribute*, 10>& objects, uint8_t scx);
  void render_scanline(uint8_t scanline);
  void render_background_scanline(uint8_t scanline);
  void render_object_scanline(uint8_t scanline);

  uint16_t get_bg_tile_map_address(uint32_t x, uint32_t y);
  uint16_t get_win_tile_map_address(uint32_t x, uint32_t y);
  uint16_t get_bgwin_tile_address(uint16_t tile_index);
  uint16_t get_obj_tile_address(uint16_t tile_index);

  void set_mode(PPUMode mode);
  void set_LY(bool force = false);
  void fire_stat_interrupt(bool previous_stat_should_fire, bool stat_interrupt_line);
  [[gnu::always_inline]] inline uint8_t LCDC() const { return ppu_registers_.get_LCDC(); }
  bool is_halted_hblank_interrupt() const;

  // Timing state
  PPUMode current_mode_ = PPUMode::OAMSearch;
  uint16_t elapsed_t_cycles_ = 0;
  uint8_t scanline_ = 0;
  uint8_t mode_3_penalty_ = 0;
  uint16_t window_scanline_ = 0;

  // Enable and status flags
  bool enabled_ = false;
  bool just_enabled_ = false;
  bool frame_just_completed_ = false;
  bool stat_interrupt_line_ = false;

  // Sub-components
  PPURegisters ppu_registers_;
  PPUMemory ppu_memory_;
  GameScreen game_screen_;
  ObjectAttributes oam_attributes_;
  Palette palette_;

  // Bridge
  PPUBridge ppu_bridge_;

  bool fire_hblank_next_tick_ = false;
};