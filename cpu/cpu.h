#pragma once

#include "clock.h"
#include "cpu_registers.h"
#include "hardware_registers.h"
#include "interrupt_controller.h"
#include "joypad.h"
#include "memory_bridge.h"
#include "memory_controller.h"
#include "program_counter.h"
#include "stack.h"

class ROMLoader;
class PPU;
class APU;
class SaveStateSerializer;

template <typename Bus>
class CPU {
public:
  CPU(ROMLoader& loader, PPU& ppu, APU& apu, Bus& bus);
  void run_single_instruction();

  void tick();

  void enable_interrupts();
  void disable_interrupts();

  void halt();
  void stop();

  void update_joypad_state(JoypadState& joypad_state);
  bool is_halted() const { return interrupts_.halt_state() == HALT; }

  // Accessors
  CPURegisters& registers() { return registers_; }
  MemoryController& mc() { return mc_; }
  HardwareRegisters& hardware_registers() { return hw_registers_; }
  ProgramCounter<Bus>& pc() { return pc_; }
  FirstLevelMemoryBridge<Bus>& memory_bridge() { return memory_bridge_; }
  Stack<Bus>& stack() { return stack_; }

  void serialize(SaveStateSerializer& serializer) const;
  void deserialize(SaveStateSerializer& serializer);

private:
  void run_next_instruction();
  void check_interrupts();
  Bus& initialise_bus(Bus& bus);

  CPURegisters registers_;
  HardwareRegisters hw_registers_;
  Timer timer_;
  MemoryController mc_;
  ProgramCounter<Bus> pc_;
  InterruptController<Bus> interrupts_;
  Stack<Bus> stack_;
  Joypad joypad_;
  PPU& ppu_;
  APU& apu_;

  FirstLevelMemoryBridge<Bus> memory_bridge_;
};

#include "cpu.inc"