# PPU Library

A self-contained Game Boy Picture Processing Unit (PPU) library with zero external dependencies. This library handles all video processing, rendering, and display logic, vram, oam and video register management for a Game Boy emulator.

## Quick Start

### Basic Usage

```cpp
#include "ppu/ppu.h"
#include "ppu/ppu_bridge.h"

// Create the bridge with callback functions
PPUBridge ppu_bridge{
    []() { /* Trigger vblank interrupt */ },
    []() { /* Trigger LCD stat interrupt */ },
    [](const uint32_t* pixels, size_t pitch) { /* Blit screen */ },
    []() -> bool { return false; /* Return true if CPU is halted */ },
    [](uint16_t addr) -> const uint8_t* { return nullptr; /* Read memory for OAM DMA */ }
};

// Create PPU instance (boot_rom_active should be true if boot ROM is active (changes what registers are initialised to))
PPU ppu(ppu_bridge, false);

//Call once per m-cycle (every 4 t-cycles)
ppu.tick();

// After each tick, you should check if a frame has completed, if it has, you should display it, perhaps after pausing for FPS reasons. 
if (ppu.frame_completed()) {
    // Frame is ready for display
}
```

## API Reference

### Constructor

```cpp
PPU(PPUBridge ppu_bridge, bool boot_rom_active);
```

Creates a new PPU instance.

**Parameters:**
- `ppu_bridge`: The `PPUBridge` contains five callback functions:
  - `trigger_vblank_interrupt()` - Called when a vblank interrupt should be triggered (IF set)
  - `trigger_lcd_stat_interrupt()` - Called when an LCD stat interrupt should be triggered (IF set)
  - `blit_screen(const uint32_t* pixels, size_t pitch)` - Called to present the rendered frame
    - `pixels`: Pointer to ARGB8888 pixel data (160x144 pixels)
    - `pitch`: Number of bytes per row (typically 160 * 4 = 640)
  - `is_halted()` - Returns `true` if the CPU is currently halted. This is needed for correct handling of delaying interrupts in halted mode
  - `read_memory(uint16_t addr)` - Returns a pointer to the byte at the given memory address. This is used for OAM DMA transfers, which read from any memory location and write to OAM
- `boot_rom_active`: Set to `true` if the boot ROM is currently active, `false` otherwise. This initializes the PPU to the correct state

### PPUBridge Interface

The `PPUBridge` struct provides the interface between the PPU and the rest of the emulator. It contains five callback functions that the PPU will invoke at appropriate times:

#### `void trigger_vblank_interrupt()`
Called at the start of V-Blank (when the PPU enters Mode 1). Your implementation should set the V-Blank interrupt flag (IF) so the CPU can handle it.

#### `void trigger_lcd_stat_interrupt()`
Called when any STAT interrupt condition is met (H-Blank, V-Blank, OAM, or LYC=LY). Your implementation should set the LCD STAT interrupt flag (IF) so the CPU can handle it.

#### `void blit_screen(const uint32_t* pixels, size_t pitch)`
Called once per frame when a complete frame has been rendered and is ready for display. The `pixels` pointer contains 160x144 ARGB8888 pixels, and `pitch` is the number of bytes per row (typically 640).

#### `bool is_halted()`
Called to check if the CPU is in halted state. This is required for accurate STAT interrupt timing on the Game Boy, as certain interrupt behaviors differ when the CPU is halted. Return `true` if the CPU is currently executing a HALT instruction.

#### `const uint8_t* read_memory(uint16_t addr)`
Called during OAM DMA transfers to read bytes from any memory location. When you write to the DMA register (0xFF46), the PPU uses this callback to read 160 bytes from the source address and copy them to OAM. This callback should return a pointer to the byte at the given address in your memory map.

### Main Methods

#### `void tick()`
Call this method **4 times per T-cycle** (or 4 times per CPU instruction). This advances the PPU state machine and handles rendering.

#### `bool frame_completed()`
Returns `true` when a complete frame has been rendered. Call this after each `tick()` to check if a new frame is ready.

### Memory Access

#### VRAM Access
```cpp
const uint8_t* read_vram(uint16_t addr) const;
void write_vram(uint16_t addr, uint8_t value);
```
- `addr`: Memory address (0x8000-0x9FFF)
- Returns pointer to VRAM byte (or writes to VRAM)

#### OAM Access
```cpp
const uint8_t* read_oam(uint16_t addr) const;
void write_oam(uint16_t addr, uint8_t value);
```
- `addr`: Memory address (0xFE00-0xFE9F)
- Automatically handles OAM DMA protection (returns garbage during restricted access)

#### Register Access
```cpp
const uint8_t* read_ppu_register(uint16_t addr) const;
void write_ppu_register(uint16_t addr, uint8_t value);
```
- `addr`: Register address (0xFF40-0xFF6C)
- Handles all PPU-specific registers including LCDC, STAT, LY, LYC, BGP, OBP0, OBP1, etc.

### Save State Management

```cpp
void serialize(SaveStateSerializer& serializer) const;
void deserialize(SaveStateSerializer& serializer);
```

These methods allow you to save and restore the complete internal state of the PPU, including:
- All PPU registers (LCDC, STAT, LY, LYC, etc.)
- VRAM contents (8KB)
- OAM contents (160 bytes)
- Internal timing state and mode
- Window line counter and other rendering state



## Integration Example

Here's a complete example of integrating the PPU into your emulator:

```cpp
#include "ppu/ppu.h"
#include "ppu/ppu_bridge.h"

class MyEmulator {
    PPU ppu_;
    
    void trigger_vblank() {
        // Your interrupt handling code
        cpu_.trigger_interrupt(INTERRUPT_VBLANK);
    }
    
    void trigger_lcd_stat() {
        // Your interrupt handling code
        cpu_.trigger_interrupt(INTERRUPT_LCD_STAT);
    }
    
    void blit_frame(const uint32_t* pixels, size_t pitch) {
        // Present frame to screen
        screen_.blit(pixels, pitch);
    }
    
    bool is_cpu_halted() {
        return cpu_.is_halted();
    }
    
    const uint8_t* read_memory(uint16_t addr) {
        return memory_.read(addr);
    }
    
public:
    MyEmulator() 
        : ppu_(PPUBridge{
            [this]() { trigger_vblank(); },
            [this]() { trigger_lcd_stat(); },
            [this](const uint32_t* pixels, size_t pitch) { blit_frame(pixels, pitch); },
            [this]() -> bool { return is_cpu_halted(); },
            [this](uint16_t addr) -> const uint8_t* { return read_memory(addr); }
          }, false)  // false = boot ROM not active
    {}
    
    void run_instruction() {
        // Execute CPU instruction
        cpu_.execute_instruction();
        
        // Tick PPU 4 times (once per T-cycle)
        for (int i = 0; i < 4; ++i) {
            ppu_.tick();
        }
        
        // Check for frame completion
        if (ppu_.frame_completed()) {
            // Frame is ready (blit_screen callback already called)
        }
    }
    
    // Route memory accesses to PPU
    void handle_memory_write(uint16_t addr, uint8_t value) {
        if (addr >= 0xFF40 && addr <= 0xFF6C) {
            ppu_.write_ppu_register(addr, value);
            // Note: The PPU handles OAM DMA transfers internally when you write
            // to the DMA register (0xFF46). It uses the read_memory callback
            // from PPUBridge to perform the transfer.
        } else if (addr >= 0x8000 && addr <= 0x9FFF) {
            ppu_.write_vram(addr, value);
        } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
            ppu_.write_oam(addr, value);
        }
    }
    
    const uint8_t* handle_memory_read(uint16_t addr) {
        if (addr >= 0xFF40 && addr <= 0xFF6C) {
            return ppu_.read_ppu_register(addr);
        } else if (addr >= 0x8000 && addr <= 0x9FFF) {
            return ppu_.read_vram(addr);
        } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
            return ppu_.read_oam(addr);
        }
        return nullptr;
    }
};
```

## Memory Map

The PPU handles the following memory ranges:

- **VRAM**: 0x8000-0x9FFF (8KB video RAM)
- **OAM**: 0xFE00-0xFE9F (160 bytes object attribute memory)
- **PPU Registers**: 0xFF40-0xFF6C
  - 0xFF40: LCDC (LCD Control)
  - 0xFF41: STAT (LCD Status)
  - 0xFF42: SCY (Scroll Y)
  - 0xFF43: SCX (Scroll X)
  - 0xFF44: LY (Current Scanline)
  - 0xFF45: LYC (LY Compare)
  - 0xFF46: DMA (OAM DMA Transfer)
  - 0xFF47: BGP (Background Palette)
  - 0xFF48: OBP0 (Object Palette 0)
  - 0xFF49: OBP1 (Object Palette 1)
  - 0xFF4A: WY (Window Y Position)
  - 0xFF4B: WX (Window X Position)
  - 0xFF68-0xFF6C: CGB-specific registers (not implemented)

## Dependencies

The PPU library has **zero external dependencies**. It only requires:
- C++20 standard library
- Headers in the `ppu/ and data_structures/` directory

