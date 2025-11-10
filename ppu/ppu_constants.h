#pragma once

#include <inttypes.h>
#include <iostream>  // IWYU pragma: keep

// Screen dimensions
constexpr uint32_t SCREEN_WIDTH = 160;
constexpr uint32_t SCREEN_HEIGHT = 144;

// Memory addresses
constexpr uint16_t VRAM_BASE_ADDRESS = 0x8000;
constexpr uint16_t VRAM_SIZE = 1024 * 8;  // 8192 bytes
constexpr uint16_t OAM_BASE_ADDRESS = 0xFE00;
constexpr uint16_t OAM_END_ADDRESS = 0xFE9F;
constexpr uint16_t OAM_SIZE = 160;
constexpr uint16_t PPU_REGISTER_START = 0xFF40;

// Register addresses
constexpr uint16_t LCDC_ADDR = 0xFF40;
constexpr uint16_t STAT_ADDR = 0xFF41;
constexpr uint16_t LY_ADDR = 0xFF44;
constexpr uint16_t LYC_ADDR = 0xFF45;
constexpr uint16_t BGP_ADDR = 0xFF47;

// Memory sizes
constexpr size_t OAM_DMA_MAX_COUNT = 4;

// Tile constants
constexpr uint8_t TILE_WIDTH = 8;
constexpr uint8_t TILE_HEIGHT = 8;
constexpr uint8_t TILE_SIZE_BYTES = 16;  // Each tile is 16 bytes (8x8 pixels, 2 bytes per row)
constexpr uint8_t TILES_PER_ROW = 32;    // 32 tiles per row in tile map

// Tile map addresses
constexpr uint16_t TILE_MAP_BASE_0 = 0x9800;
constexpr uint16_t TILE_MAP_BASE_1 = 0x9C00;
constexpr uint16_t TILE_DATA_BASE_0 = 0x8000;
constexpr uint16_t TILE_DATA_BASE_1 = 0x9000;

// Timing constants (in T-cycles)
constexpr uint16_t OAM_SEARCH_CYCLES = 80;
constexpr uint16_t PIXEL_TRANSFER_BASE_CYCLES = 172;
constexpr uint16_t HBLANK_BASE_CYCLES = 204;
constexpr uint16_t SCANLINE_CYCLES = 456;
constexpr uint16_t T_CYCLES_PER_TICK = 4;

// Scanline constants
constexpr uint8_t VBLANK_START_LINE = 144;
constexpr uint8_t VBLANK_END_LINE = 154;

// Object/Sprite constants
constexpr uint8_t MAX_SPRITES_PER_SCANLINE = 10;
constexpr uint8_t TOTAL_SPRITES = 40;
constexpr int8_t SPRITE_X_OFFSET = 8;   // Sprite X position offset
constexpr int8_t SPRITE_Y_OFFSET = 16;  // Sprite Y position offset
constexpr uint8_t SPRITE_HEIGHT_8X8 = 7;
constexpr uint8_t SPRITE_HEIGHT_8X16 = 15;
constexpr uint8_t TILE_PIXELS_PER_ROW = 8;

// Mode 3 penalty
constexpr uint8_t MODE_3_WINDOW_SWITCH_PENALTY = 6;
constexpr uint8_t MODE_3_SCX_PENALTY_DIVISOR = 8;

// Window constants
constexpr uint8_t WINDOW_X_OFFSET = 7;
constexpr uint8_t WINDOW_MAX_X = 167;

// STAT register bit masks
constexpr uint8_t STAT_MODE_MASK = 0x03;        // Bits 0-1: Mode
constexpr uint8_t STAT_LYC_FLAG = 0x04;         // Bit 2: LYC=LY flag
constexpr uint8_t STAT_HBLANK_INT = 0x08;       // Bit 3: HBlank interrupt
constexpr uint8_t STAT_VBLANK_INT = 0x10;       // Bit 4: VBlank interrupt
constexpr uint8_t STAT_OAM_INT = 0x20;          // Bit 5: OAM interrupt
constexpr uint8_t STAT_LYC_INT = 0x40;          // Bit 6: LYC interrupt
constexpr uint8_t STAT_WRITABLE_MASK = 0x78;    // Bits 3-6: Writable
constexpr uint8_t STAT_READONLY_MASK = 0x07;    // Bits 0-2: Read-only
constexpr uint8_t STAT_BIT_7_SET = 0x80;        // Bit 7: Always set
constexpr uint8_t STAT_MODE_CLEAR_MASK = 0xFC;  // Clear mode bits

// LCDC register bit masks
constexpr uint8_t LCDC_BG_ENABLE = 0x01;        // Bit 0: BG display enable
constexpr uint8_t LCDC_SPRITE_ENABLE = 0x02;    // Bit 1: Sprite enable
constexpr uint8_t LCDC_SPRITE_SIZE = 0x04;      // Bit 2: Sprite size (0=8x8, 1=8x16)
constexpr uint8_t LCDC_BG_TILE_MAP = 0x08;      // Bit 3: BG tile map (0=0x9800, 1=0x9C00)
constexpr uint8_t LCDC_TILE_DATA = 0x10;        // Bit 4: Tile data (0=0x9000, 1=0x8000)
constexpr uint8_t LCDC_WINDOW_ENABLE = 0x20;    // Bit 5: Window enable
constexpr uint8_t LCDC_WINDOW_TILE_MAP = 0x40;  // Bit 6: Window tile map (0=0x9800, 1=0x9C00)
constexpr uint8_t LCDC_DISPLAY_ENABLE = 0x80;   // Bit 7: Display enable

// PPU mode constants (matching PPUMode enum)
constexpr uint8_t PPU_MODE_HBLANK = 0;
constexpr uint8_t PPU_MODE_VBLANK = 1;
constexpr uint8_t PPU_MODE_OAM_SEARCH = 2;
constexpr uint8_t PPU_MODE_PIXEL_TRANSFER = 3;

// PPU enable timing
constexpr uint16_t PPU_ENABLE_OFFSET_CYCLES = 204 - 76;  // Cycles when PPU is enabled

// OAMDMA constants
constexpr uint8_t OAMDMA_WAIT_CYCLES = 2;
constexpr uint8_t OAMDMA_GARBAGE_VALUE = 0xFF;

// Debug output
constexpr bool PPU_VERBOSE = false;

// Verbose print macro for PPU
#define PPU_VERBOSE_PRINT()  \
  if constexpr (PPU_VERBOSE) \
  std::cout
