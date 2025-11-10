#pragma once

#include <cstdint>

#include "detail/memory_bridge_components.h"

/*
This code creates two arrays of handlers each for reading and writing to memory.

The first array is 128 handlers, and memory is mapped to each handler by dividing the address by 0x200 (512)
This maps neatly for every memory range except for the last one, which needs further division.

The second array is 512 handlers and covers 0xFE00 -> 0xFFFF, and memory is mapped directly to address minus 0xFE00.


*/

template <typename Bus>
struct ROM0Handler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->memory_controller_->read_rom0(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_rom0(addr, value); }
};

template <typename Bus>
struct ROM1Handler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->memory_controller_->read_rom1(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_rom1(addr, value); }
};

template <typename Bus>
struct VRAMHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->ppu_->read_vram(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->ppu_->write_vram(addr, value); }
};
template <typename Bus>
struct RAMHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->memory_controller_->read_ram(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_ram(addr, value); }
};
template <typename Bus>
struct WRAMHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->memory_controller_->read_wram(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_wram(addr, value); }
};
template <typename Bus>
struct ECHOHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->cpu_->memory_bridge().read(addr - 0x2000); }
  void write(uint16_t addr, uint8_t value, Bus* bus) {
    bus->cpu_->memory_bridge().write(addr - 0x2000, value);
  }
};
template <typename Bus>
struct SecondLevelHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) {
    return bus->cpu_->memory_bridge().late_range_memory_bridge().read(addr);
  }
  void write(uint16_t addr, uint8_t value, Bus* bus) {
    bus->cpu_->memory_bridge().late_range_memory_bridge().write(addr, value);
  }
};

template <typename Bus>
struct OAMHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->ppu_->read_oam(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->ppu_->write_oam(addr, value); }
};

template <typename Bus>
struct HRAMHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->memory_controller_->read_hram(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_hram(addr, value); }
};

template <typename Bus>
struct DIVHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->timer_->get_div(); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->timer_->write_div(); }
};
template <typename Bus>
struct AudioHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->apu_->audio_register_read(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->apu_->audio_register_write(addr, value); }
};
template <typename Bus>
struct IEHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return &bus->hw_registers_->get_IE(); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->hw_registers_->set_IE(value); }
};
template <typename Bus>
struct HardwareRegisterHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return &bus->hw_registers_->read_register(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->hw_registers_->write_register(addr, value); }
};

template <typename Bus>
struct JoypadNotifyHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->joypad_->joypad_write(); }
};

template <typename Bus>
struct NotifyWriteHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->write_callback(addr, value); }
};

template <typename Bus>
struct OAMDMAStartHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) {}
};
template <typename Bus>
struct PPURegisterHandler {
  const uint8_t* read(uint16_t addr, Bus* bus) { return bus->ppu_->read_ppu_register(addr); }
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->ppu_->write_ppu_register(addr, value); }
};

template <typename Bus>
struct BootROMHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->memory_controller_->unload_boot_rom(); }
};

template <typename Bus>
struct TimaWriteHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->timer_->write_tima(value); }
};

template <typename Bus>
struct TmaWriteHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->timer_->write_tma(value); }
};

template <typename Bus>
struct TacWriteHandler {
  void write(uint16_t addr, uint8_t value, Bus* bus) { bus->timer_->write_tac(value); }
};

// Define your memory map with ranges and specific addresses
template <typename Bus>
using FirstLevelReadHandlers =
    ReadHandlerList<Bus, FirstLevelRangeCallbacks<0x0000, 0x3FFF, ROM0Handler<Bus>>,
                    FirstLevelRangeCallbacks<0x4000, 0x7FFF, ROM1Handler<Bus>>,
                    FirstLevelRangeCallbacks<0x8000, 0x9FFF, VRAMHandler<Bus>>,
                    FirstLevelRangeCallbacks<0xA000, 0xBFFF, RAMHandler<Bus>>,
                    FirstLevelRangeCallbacks<0xC000, 0xDFFF, WRAMHandler<Bus>>,
                    FirstLevelRangeCallbacks<0xE000, 0xFDFF, ECHOHandler<Bus>>,
                    FirstLevelRangeCallbacks<0xFE00, 0xFFFF, SecondLevelHandler<Bus>>>;

template <typename Bus>
using FirstLevelWriteHandlers =
    WriteHandlerList<Bus, FirstLevelRangeCallbacks<0x0000, 0x3FFF, ROM0Handler<Bus>>,
                     FirstLevelRangeCallbacks<0x4000, 0x7FFF, ROM1Handler<Bus>>,
                     FirstLevelRangeCallbacks<0x8000, 0x9FFF, VRAMHandler<Bus>>,
                     FirstLevelRangeCallbacks<0xA000, 0xBFFF, RAMHandler<Bus>>,
                     FirstLevelRangeCallbacks<0xC000, 0xDFFF, WRAMHandler<Bus>>,
                     FirstLevelRangeCallbacks<0xE000, 0xFDFF, ECHOHandler<Bus>>,
                     FirstLevelRangeCallbacks<0xFE00, 0xFFFF, SecondLevelHandler<Bus>>>;

template <typename Bus>
using SecondLevelReadHandlers = ReadHandlerList<
    Bus, SecondLevelRangeCallbacks<0xFE00, 0xFE9F, OAMHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF80, 0xFFFE, HRAMHandler<Bus>>, AddressCallbacks<0xFF04, DIVHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF10, 0xFF3F, AudioHandler<Bus>>, AddressCallbacks<0xFFFF, IEHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF00, 0xFF03, HardwareRegisterHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF05, 0xFF0F, HardwareRegisterHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF40, 0xFF6C, PPURegisterHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF6D, 0xFF7F, HardwareRegisterHandler<Bus>>>;

template <typename Bus>
using SecondLevelWriteHandlers = WriteHandlerList<
    Bus, SecondLevelRangeCallbacks<0xFE00, 0xFE9F, OAMHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF80, 0xFFFE, HRAMHandler<Bus>>, AddressCallbacks<0xFFFF, IEHandler<Bus>>,
    AddressCallbacks<0xFF00, HardwareRegisterHandler<Bus>, JoypadNotifyHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF01, 0xFF03, HardwareRegisterHandler<Bus>>,
    AddressCallbacks<0xFF02, NotifyWriteHandler<Bus>>, AddressCallbacks<0xFF04, DIVHandler<Bus>>,
    AddressCallbacks<0xFF05, TimaWriteHandler<Bus>>, AddressCallbacks<0xFF06, TmaWriteHandler<Bus>>,
    AddressCallbacks<0xFF07, TacWriteHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF08, 0xFF0F, HardwareRegisterHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF10, 0xFF3F, AudioHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF40, 0xFF6C, PPURegisterHandler<Bus>>,
    SecondLevelRangeCallbacks<0xFF6D, 0xFF7F, HardwareRegisterHandler<Bus>>,
    AddressCallbacks<0xFF50, BootROMHandler<Bus>>>;

template <typename Bus>
class LateRangeMemoryBridge : public SecondLevelMemoryBridge<Bus, typename SecondLevelReadHandlers<Bus>::type,
                                                             typename SecondLevelWriteHandlers<Bus>::type> {
public:
  using SecondLevelMemoryBridge<Bus, typename SecondLevelReadHandlers<Bus>::type,
                                typename SecondLevelWriteHandlers<Bus>::type>::SecondLevelMemoryBridge;
};

template <typename Bus>
class FirstLevelMemoryBridge : public MemoryBridge<Bus, typename FirstLevelReadHandlers<Bus>::type,
                                                   typename FirstLevelWriteHandlers<Bus>::type> {
public:
  FirstLevelMemoryBridge(Bus* bus)
      : MemoryBridge<Bus, typename FirstLevelReadHandlers<Bus>::type,
                     typename FirstLevelWriteHandlers<Bus>::type>(bus),
        late_range_memory_bridge_(bus) {}

  LateRangeMemoryBridge<Bus>& late_range_memory_bridge() { return late_range_memory_bridge_; }

private:
  LateRangeMemoryBridge<Bus> late_range_memory_bridge_;
};