#include <instructions/instruction_decoder.h>
#include <cstdint>
#include "apu.h"
#include "joypad_state.h"
#include "ppu.h"
#include "rom_loader.h"
#include "save_state.h"

template <typename Bus>
CPU<Bus>::CPU(ROMLoader& loader, PPU& ppu, APU& apu, Bus& bus)
    : registers_(loader.has_boot_rom()),
      timer_(hw_registers_),
      mc_(loader),
      pc_(registers_, memory_bridge_),
      interrupts_(hw_registers_, pc_),
      stack_(memory_bridge_, registers_),
      joypad_(hw_registers_),
      ppu_(ppu),
      apu_(apu),
      memory_bridge_(&initialise_bus(bus)) {
  timer_.set_apu_callback([&]() { apu_.tick_frame_sequencer(); });
}

template <typename Bus>
Bus& CPU<Bus>::initialise_bus(Bus& bus) {
  bus.ppu_ = &ppu_;
  bus.apu_ = &apu_;
  bus.cpu_ = this;
  bus.timer_ = &timer_;
  bus.hw_registers_ = &hw_registers_;
  bus.memory_controller_ = &mc_;
  bus.joypad_ = &joypad_;
  return bus;
}

template <typename Bus>
void CPU<Bus>::run_single_instruction() {
  check_interrupts();

  if (interrupts_.should_execute_instruction()) {
    run_next_instruction();
  } else {
    tick();
  }
}

template <typename Bus>
void CPU<Bus>::tick() {
  timer_.tick();
  ppu_.tick();
  apu_.tick();
  mc_.tick();
  interrupts_.check_for_enable();
}

template <typename Bus>
void CPU<Bus>::run_next_instruction() {
  const static auto clear_halt_bug = [this]() {
    interrupts_.clear_halt_bug();
  };
  const uint8_t opcode = *pc_.fetch_instruction(interrupts_.halt_state(), clear_halt_bug);
  InstructionDecoder::decode_and_execute(opcode, this);
}

template <typename Bus>
void CPU<Bus>::check_interrupts() {
  if (interrupts_.service_interrupts([this](uint8_t value) { stack_.push_8(value); })) {
    tick();
    tick();
    tick();
    tick();
    tick();
  }
}

template <typename Bus>
void CPU<Bus>::enable_interrupts() {
  interrupts_.enable_interrupts();
}

template <typename Bus>
void CPU<Bus>::disable_interrupts() {
  interrupts_.disable_interrupts();
}

template <typename Bus>
void CPU<Bus>::halt() {
  interrupts_.halt();
}

template <typename Bus>
void CPU<Bus>::stop() {
  interrupts_.stop();
}

template <typename Bus>
void CPU<Bus>::update_joypad_state(JoypadState& joypad_state) {
  joypad_.update_state(joypad_state);
}

template <typename Bus>
void CPU<Bus>::serialize(SaveStateSerializer& serializer) const {
  serializer << registers_;
  serializer << hw_registers_;
  serializer << mc_;
  serializer << interrupts_;
  serializer << timer_;
}

template <typename Bus>
void CPU<Bus>::deserialize(SaveStateSerializer& serializer) {
  serializer >> registers_;
  serializer >> hw_registers_;
  serializer >> mc_;
  serializer >> interrupts_;
  serializer >> timer_;
}